#include <Arduino.h>
#include <utility>
#include "Globals.hpp"
#include "HardwareSerial.h"
#include "MyTasks.hpp"
#include "Btn.hpp"
#include "Ton.hpp"
#include "DCMotor.hpp" 
#include "esp32-hal-adc.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"

int step = 0;
TON ton_blink1(500);
TON ton_entry_part(5000);
TON ton_type_part(5000);
TON ton_exit_part(5000);
unsigned long timeout_blink_at_entry;
unsigned long timeout_part_at_entry;
unsigned long timeout_part_type;
unsigned long timeout_part_at_exit;

Btn btn_start_process(INIT_PROCESS, false);
Btn btn_part_in_position(PART_AT_ENTRY, false);
Btn btn_part_in_exit(PART_AT_EXIT, false);
Btn btn_is_part_type1(PART_TYPE1, false);
Btn btn_is_part_type2(PART_TYPE2, false);
Btn btn_is_part_type3(PART_TYPE3, false);
DCMotor motor_conveyor(DIR1_MOTOR, DIR2_MOTOR, PWM_SPEED_MOTOR);

enum AlarmCode {
    ALARM_NONE = 0,
    ALARM_TIMEOUT_ENTRADA_PECA = 10,
    ALARM_TIMEOUT_CHECAGEM_TIPO = 20,
    ALARM_TIMEOUT_SAIDA_PECA = 30
};

void taskConveyor(void *pvParameters)
{
    // Inicialização
    ton_blink1.reset();
    ton_entry_part.reset();
    ton_type_part.reset();
    ton_exit_part.reset();

    motor_conveyor.setSpeed(255);
    motor_conveyor.Stop();
    
    HREGS_MAP[OUT_SPEED_MOTOR]      = 0;
    HREGS_MAP[OUT_QTY_PART_TYPE1]   = 0;
    HREGS_MAP[OUT_QTY_PART_TYPE2]   = 0;
    HREGS_MAP[OUT_QTY_PART_TYPE3]   = 0;
    HREGS_MAP[OUT_ACTUAL_ALARM]     = ALARM_NONE; 
    
    motor_conveyor.setRampParameters(30, 3);
    
    delay(2000);
    Serial.println("Task Conveyor Iniciada");

    while (true)
    {
        // DEBUG: Verificar estados
        static unsigned long lastDebug = 0;
        if (millis() - lastDebug > 2000) {
            Serial.print("Step: ");
            Serial.print(step);
            Serial.print(" | Blink: ");
            Serial.print(ton_blink1.done());
            Serial.print(" | Entry: ");
            Serial.print(ton_entry_part.done());
            Serial.print(" | Type: ");
            Serial.print(ton_type_part.done());
            Serial.print(" | Exit: ");
            Serial.println(ton_exit_part.done());
            lastDebug = millis();
        }

        if (btn_start_process.press()) {
            Serial.println("=== BOTÃO START PRESSIONADO ===");
            // RESET COMPLETO de todos os TONs
            ton_blink1.reset();
            ton_entry_part.reset();
            ton_type_part.reset();
            ton_exit_part.reset();
            
            HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE;
            Serial.println("INICIANDO PROCESSO - STEP 1");             
            timeout_blink_at_entry = millis();
            timeout_part_at_entry = millis();
            step = 1;
        }

        switch (step) {
            case 0:
                // Estado de repouso - para tudo
                motor_conveyor.Stop();
                digitalWrite(STATUS_MACHINE, LOW);
                digitalWrite(STATUS_MOTOR, LOW);
                
                // Garante que todos os TONs estão resetados
                if (ton_blink1.isEnabled() || ton_entry_part.isEnabled() || 
                    ton_type_part.isEnabled() || ton_exit_part.isEnabled()) {
                    ton_blink1.reset();
                    ton_entry_part.reset();
                    ton_type_part.reset();
                    ton_exit_part.reset();
                    Serial.println("Reset completo no estado 0");
                }
                timeout_part_type = millis();
                timeout_part_at_entry = millis();
                timeout_blink_at_entry = millis();
                timeout_part_at_exit = millis();
                break;

            case 1: // Aguarda chegada de peça na entrada
                {
                    // Atualiza velocidade do motor (mas não movimenta ainda)
                    int speed = map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255);
                    motor_conveyor.setSpeed(speed);
                    HREGS_MAP[OUT_SPEED_MOTOR] = speed;
                    motor_conveyor.Stop(); // Para na posição inicial
                    
                    // Pisca lâmpada status máquina - SOMENTE no step 1
                    //if (ton_blink1.update(true))
                    if (millis() - timeout_blink_at_entry >= 500)
                    {
                        digitalWrite(STATUS_MACHINE, !digitalRead(STATUS_MACHINE));
                        Serial.println("STEP 1 - Piscando LED - Aguardando peça");
                        timeout_blink_at_entry = millis();
                    }
                    
                    // Timer de timeout para entrada - SOMENTE quando aguardando peça
                    //if (ton_entry_part.update(true)) {
                    if (millis() - timeout_part_at_entry >= 3000)
                    {
                        // Timer completou - timeout
                        Serial.println("TIMEOUT: Peça não chegou na entrada");
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_ENTRADA_PECA;
                        digitalWrite(STATUS_MACHINE, LOW);
                        step = 0;
                    }
                    
                    // Verifica se peça chegou
                    //if (btn_part_in_position.press())
                    if ((millis() - timeout_part_at_entry < 3000) && btn_part_in_position.press())
                    {
                        Serial.println("PEÇA DETECTADA NA ENTRADA!");
                        digitalWrite(STATUS_MACHINE, HIGH); // LED fixo
                        ton_blink1.reset(); // Para de piscar
                        ton_entry_part.reset(); // Reseta timer de entrada
                        timeout_part_type = millis();
                        step = 10;
                    }
                }
                break;
                
            case 10: // Aguarda classificação do tipo de peça
                {
                    // Movimenta a esteira
                    int speed = map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255);
                    motor_conveyor.setSpeed(speed);
                    motor_conveyor.Forward();
                    motor_conveyor.update();
                    HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                    digitalWrite(STATUS_MOTOR, HIGH);
                    digitalWrite(STATUS_MACHINE, HIGH); // LED permanece aceso

                    // Para o blink no step 10
                    ton_blink1.reset();
                    digitalWrite(STATUS_MACHINE, HIGH);

                    // Verifica tipo de peça
                    unsigned short part_detected = 0;
                    if (btn_is_part_type1.press()) {
                        Serial.println("PEÇA TIPO 1 DETECTADA");
                        part_detected = 1;
                    }
                    else if (btn_is_part_type2.press()) {
                        Serial.println("PEÇA TIPO 2 DETECTADA");
                        part_detected = 2;
                    }
                    else if (btn_is_part_type3.press()) {
                        Serial.println("PEÇA TIPO 3 DETECTADA");
                        part_detected = 3;
                    }

                    // Timeout para detecção do tipo
                    //if (ton_type_part.update(true))
                    if (millis() - timeout_part_type >= 3000)
                    {
                        // Timeout ocorreu
                        Serial.println("TIMEOUT: Tipo de peça não detectado");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_CHECAGEM_TIPO;
                        ton_type_part.reset();
                        step = 0;
                    }
                    
                    // Peça detectada com sucesso
                    //if (part_detected != 0)
                    if (part_detected != 0 && (millis() - timeout_part_type < 3000))
                    {
                        Serial.print("PEÇA CONFIRMADA: Tipo ");
                        Serial.println(part_detected);
                        
                        // Incrementa contador
                        switch (part_detected) {
                            case 1: 
                                HREGS_MAP[OUT_QTY_PART_TYPE1]++; 
                                break;
                            case 2: 
                                HREGS_MAP[OUT_QTY_PART_TYPE2]++; 
                                break;
                            case 3: 
                                HREGS_MAP[OUT_QTY_PART_TYPE3]++; 
                                break;
                        }
                        
                        ton_type_part.reset(); // Reseta timer de tipo
                        timeout_part_at_exit = millis();
                        step = 20;
                    }
                }
                break;

            case 20: // Aguarda saída da peça
                {
                    // Continua movimentação
                    int speed = map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255);
                    motor_conveyor.setSpeed(speed);
                    motor_conveyor.Forward();
                    motor_conveyor.update();
                    HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                    digitalWrite(STATUS_MOTOR, HIGH);
                    digitalWrite(STATUS_MACHINE, HIGH);

                    // Timeout para saída
                    //if (ton_exit_part.update(true))
                    if (millis() - timeout_part_at_exit >= 3000)
                    {
                        Serial.println("TIMEOUT: Peça não saiu");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_SAIDA_PECA;
                        ton_exit_part.reset();
                        step = 0;
                    }
                    
                    // Peça saiu com sucesso
                    //if (btn_part_in_exit.press())
                    if ((millis() - timeout_part_at_exit < 3000) && btn_part_in_exit.press())
                    {
                        Serial.println("PEÇA SAIU DA ESTEIRA - PROCESSO CONCLUÍDO");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        ton_exit_part.reset(); // Reseta timer de saída
                        step = 0;
                    }
                }
                break;
        }
        
        vTaskDelay(10);
    }
}
