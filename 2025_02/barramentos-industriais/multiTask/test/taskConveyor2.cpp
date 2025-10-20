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

int last_cont = 0;
int step = 0;
unsigned long timeout_part_type = 5000;
TON ton_blink1(500);
TON ton_entry_part(5000);
TON ton_type_part(5000);
TON ton_exit_part(5000);

Btn btn_start_process(INIT_PROCESS, false);
Btn btn_part_in_position(PART_AT_ENTRY, false);
Btn btn_part_in_exit(PART_AT_EXIT, false);
Btn btn_is_part_type1(PART_TYPE1, false);
Btn btn_is_part_type2(PART_TYPE2, false);
Btn btn_is_part_type3(PART_TYPE3, false);
DCMotor motor_conveyor(DIR1_MOTOR, DIR2_MOTOR, PWM_SPEED_MOTOR);
unsigned long time1 = 0;

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
    
    time1 = millis();
    delay(2000);

    while (true)
    {
        // DEBUG: Verificar se o botão está sendo detectado
        if (btn_start_process.press()) {
            Serial.println("BOTÃO START PRESSIONADO!");
            ton_blink1.reset();
            ton_entry_part.reset();
            ton_type_part.reset();
            ton_exit_part.reset();
            step = 1;
            HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE;
            Serial.println("INICIANDO PROCESSO - STEP 1");             
        }

        switch (step) {
            case 0:
                // Estado de repouso
                ton_blink1.reset();
                ton_entry_part.reset();
                ton_type_part.reset();
                ton_exit_part.reset();
                motor_conveyor.Stop();
                digitalWrite(STATUS_MACHINE, LOW);
                digitalWrite(STATUS_MOTOR, LOW);
                break; // ←←←←←←←←←←←←←←←← BREAK ADICIONADO

            case 1: // Aguarda chegada de peça na entrada
                {
                    // Atualiza velocidade do motor
                    int speed = map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255);
                    motor_conveyor.setSpeed(speed);
                    HREGS_MAP[OUT_SPEED_MOTOR] = speed;
                    
                    // Pisca lâmpada status máquina
                    if(ton_blink1.update(true)) {
                        digitalWrite(STATUS_MACHINE, !digitalRead(STATUS_MACHINE));
                        Serial.println("STEP 1 - Aguardando peça na entrada");
                    }
                    
                    // Inicia timer de timeout para entrada
                    ton_entry_part.update(true);
                    
                    // Verifica timeout
                    if (ton_entry_part.done()) {
                        Serial.println("TIMEOUT: Peça não chegou na entrada");
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_ENTRADA_PECA;
                        digitalWrite(STATUS_MACHINE, LOW);
                        step = 0;
                    }
                    
                    // Verifica se peça chegou
                    if (btn_part_in_position.press()) {
                        Serial.println("PEÇA DETECTADA NA ENTRADA!");
                        digitalWrite(STATUS_MACHINE, HIGH);
                        ton_entry_part.reset();
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

                    // Timeout para detecção
                    ton_type_part.update(true);
                    
                    if (ton_type_part.done()) {
                        Serial.println("TIMEOUT: Tipo de peça não detectado");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_CHECAGEM_TIPO;
                        step = 0;
                    }
                    
                    // Peça detectada
                    if (part_detected != 0) {
                        Serial.print("PEÇA CONFIRMADA: Tipo ");
                        Serial.println(part_detected);
                        
                        switch (part_detected) {
                            case 1: HREGS_MAP[OUT_QTY_PART_TYPE1]++; break;
                            case 2: HREGS_MAP[OUT_QTY_PART_TYPE2]++; break;
                            case 3: HREGS_MAP[OUT_QTY_PART_TYPE3]++; break;
                        }
                        
                        ton_type_part.reset();
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

                    // Timeout para saída
                    ton_exit_part.update(true);
                    
                    if (ton_exit_part.done()) {
                        Serial.println("TIMEOUT: Peça não saiu");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_SAIDA_PECA;
                        step = 0;
                    }
                    
                    // Peça saiu
                    if (btn_part_in_exit.press()) {
                        Serial.println("PEÇA SAIU DA ESTEIRA - PROCESSO CONCLUÍDO");
                        motor_conveyor.Stop();
                        digitalWrite(STATUS_MACHINE, LOW);
                        digitalWrite(STATUS_MOTOR, LOW);
                        step = 0;
                    }
                }
                break;
        }
        
        vTaskDelay(10); // Pequeno delay para não sobrecarregar
    }
}
