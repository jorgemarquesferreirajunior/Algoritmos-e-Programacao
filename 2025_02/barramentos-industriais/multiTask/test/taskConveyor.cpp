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
unsigned long timeout_part_type = 5000; // tempo limite para deteccao de algum tipo de peca
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
    ALARM_TIMEOUT_ENTRADA_PECA = 10,      // Falha timeout aguardo de peça entrada da esteira
    ALARM_TIMEOUT_CHECAGEM_TIPO = 20,     // Falha timeout checagem tipo de peça
    ALARM_TIMEOUT_SAIDA_PECA = 30         // Falha timeout saída de peça
};

void taskConveyor(void *pvParameters)
{
    ton_blink1.reset();
    ton_entry_part.reset();
    ton_type_part.reset();
    ton_exit_part.reset();

    motor_conveyor.setSpeed(255);
    motor_conveyor.Stop();
    
    HREGS_MAP[OUT_SPEED_MOTOR]      = motor_conveyor.getCurrentSpeed(); 
    HREGS_MAP[OUT_QTY_PART_TYPE1]   = 0;
    HREGS_MAP[OUT_QTY_PART_TYPE2]   = 0;
    HREGS_MAP[OUT_QTY_PART_TYPE3]   = 0;
    HREGS_MAP[OUT_ACTUAL_ALARM]     = ALARM_NONE; 
    
    motor_conveyor.setRampParameters(30,3);//30ms de passo, incremento de 3
    HREGS_MAP[OUT_SPEED_MOTOR] = analogRead(IN_SPEED_MOTOR);

    time1 = millis();
    delay(2000);

    while (true)
    {
        /*int temp;
        temp = 0;

        if (millis() - time1 >= 1000){
        for (int i = 0; i < N_DINPUTS; i++) {
            Serial.print(DINPUTS_MAP[i]);
            i < 10 ? Serial.print(" : ") : Serial.print(": ");
            Serial.println(!digitalRead(DINPUTS_MAP[i]));
        }
        temp =  (!digitalRead(DINPUTS_MAP[N_DINPUTS-3]) << 0) | (!digitalRead(DINPUTS_MAP[N_DINPUTS-2]) << 1)| (!digitalRead(DINPUTS_MAP[N_DINPUTS-1]) << 2);
        Serial.print("SLAVE: ");
        Serial.print(temp);
        Serial.println("");
        time1 = millis();
        }*/


        if (btn_start_process.press())
        {
            ton_blink1.reset();
            ton_entry_part.reset();
            ton_type_part.reset();
            ton_exit_part.reset();
            step = 1;
            HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE;             
        }
        switch (step) {
            case 0:
            ton_blink1.reset();
            ton_entry_part.reset();
            ton_type_part.reset();
            ton_exit_part.reset();

            case 1:// aguarda chegada de peca na entrada da esteira

                motor_conveyor.setSpeed(map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255));
                digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());
                // motor_conveyor.update();
                // motor_conveyor.Forward();
                HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());

                ton_entry_part.update(!ton_entry_part.done());
                // pisca lampada status maquina
                if(ton_blink1.update(!ton_blink1.done()))
                {
                    Serial.println("STEP 1");
                    digitalWrite(STATUS_MACHINE, !digitalRead(STATUS_MACHINE));
                }
                
                // timeout sensor presenca peca na entrada
                if (ton_entry_part.done())
                {
                    Serial.println("TIMEOUT PRESENCA PECA NA ENTRADA");
                    HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE; 
                    digitalWrite(STATUS_MACHINE, LOW);
                    step = 0;
                }
                // peca chegou no sensor de entrada antes do timeout
                if (btn_part_in_position.press() && ton_entry_part.isEnabled() && !ton_entry_part.done()) {
                    digitalWrite(STATUS_MACHINE, HIGH);
                    step = 10;
                    // ligar motor
}
                break;
                
            case 10: // aguarda classificacao do tipo de peca detec
                
                motor_conveyor.setSpeed(map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255));
                motor_conveyor.Forward();
                motor_conveyor.update();
                HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());


                ton_blink1.reset();
                ton_entry_part.reset();

                unsigned short part_detected;
                part_detected = 0;
                if (btn_is_part_type1.press())
                {
                    Serial.println("PECA 1 DETECTADA");
                    part_detected = 1;
                }
                else if (btn_is_part_type2.press()) {
                    Serial.println("PECA 2 DETECTADA");
                    part_detected = 2;
                }
                else if (btn_is_part_type3.press()) {
                    Serial.println("PECA 3 DETECTADA");
                    part_detected = 3;
                }
                

                // timeout deteccao de peca
                if(ton_type_part.update(!ton_type_part.done()))
                {
                    Serial.println("TIMEOUT TIPO DE PECA");
                    motor_conveyor.Stop();
                    digitalWrite(STATUS_MACHINE, LOW);
                    digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());
                    HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                    HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_CHECAGEM_TIPO;
                    step = 0;
                }
                // peca detectada antes do timeout
                if (part_detected != 0 && !ton_type_part.done())
                {
                    Serial.print("PECA DETECTADA: ");
                    Serial.println(part_detected);

                    switch (part_detected)
                    {
                        case 1:
                            HREGS_MAP[PART_TYPE1]++;
                            break;
                        case 2:
                            HREGS_MAP[PART_TYPE2]++;
                            break;
                        case 3:
                            HREGS_MAP[PART_TYPE3]++;
                            break;
                        default:
                            break;
                    }

                    step = 20;
                }
            break;

            case 20: // aguarda saida de peca no final da esteira
                
                ton_type_part.reset();
                motor_conveyor.setSpeed(map(analogRead(IN_SPEED_MOTOR), 0, 1023, 0, 255));
                motor_conveyor.Forward();
                motor_conveyor.update();
                HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());


                // timeout saida peca
                if(ton_exit_part.update(!ton_exit_part.done()))
                {
                    motor_conveyor.Stop();
                    digitalWrite(STATUS_MACHINE, LOW);
                    digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());
                    HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                    HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_TIMEOUT_SAIDA_PECA;
                    step = 0;
                }
                // peca chegou no final da esteira
                if(btn_part_in_exit.press() && ton_exit_part.isEnabled() && !ton_exit_part.done())
                {
                    motor_conveyor.Stop();
                    digitalWrite(STATUS_MACHINE, LOW);
                    digitalWrite(STATUS_MOTOR, motor_conveyor.isWorking());
                    HREGS_MAP[OUT_SPEED_MOTOR] = motor_conveyor.getCurrentSpeed();
                    step = 0;
                }
            break;
        }
        vTaskDelay(1);
    }
}


