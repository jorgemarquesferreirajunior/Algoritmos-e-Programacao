#include <Arduino.h>
#include "Globals.hpp"
#include "HardwareSerial.h"
#include "MyTasks.hpp"
#include "Btn.hpp"
#include "Ton.hpp"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"

int last_cont = 0;
int step = 0;
unsigned long timeout_part_type = 5000; // tempo limite para deteccao de algum tipo de peca
TON ton_blink1(500);
TON ton_entry_part(5000);
TON ton_type_part(5000);
TON ton_exit_part(5000);

Btn btn_start_process(INIT_PROCESS, true);
Btn btn_part_in_position(PART_AT_ENTRY, true);
Btn btn_part_in_exit(PART_AT_EXIT, true);
Btn btn_is_part_type1(PART_TYPE1, true);
Btn btn_is_part_type2(PART_TYPE2, true);
Btn btn_is_part_type3(PART_TYPE3, true);
unsigned long time1 = 0;
bool temp1 = false;

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
    HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE; 
    HREGS_MAP[OUT_SPEED_MOTOR] = 0; 
    COILS_MAP[STATUS_MOTOR] = LOW;

    while (true)
    {
        if (btn_start_process.press())
        {
            step = 1;
            HREGS_MAP[OUT_ACTUAL_ALARM] = ALARM_NONE; 
            HREGS_MAP[OUT_SPEED_MOTOR] = analogRead(IN_SPEED_MOTOR);
            COILS_MAP[STATUS_MOTOR] = HIGH;
        }
        switch (step) {
            case 1:// aguarda chegada de peca na entrada da esteira
                ton_entry_part.update(!ton_entry_part.done());
                if(ton_blink1.update(!ton_blink1.done()))
                {
                    digitalWrite(STATUS_MACHINE, !digitalRead(STATUS_MACHINE));
                }
                
                // timeout sensor presenca peca na entrada
                if (ton_entry_part.done())
                {
                    step = 0;
                    HREGS_MAP[4] = ALARM_TIMEOUT_ENTRADA_PECA;
                    digitalWrite(STATUS_MACHINE, LOW);
                }
                if (btn_part_in_position.press() && !ton_entry_part.done()) {
                    digitalWrite(STATUS_MACHINE, HIGH);
                    step = 10;
                    // ligar motor
                }
                break;
                
            case 10: // aguarda classificacao do tipo de peca detectada
                ton_blink1.reset();
                ton_entry_part.reset();
                bool part_detected;
                part_detected = btn_is_part_type1.press() || btn_is_part_type2.press() || btn_is_part_type3.press();

                if(ton_entry_part.update(!ton_entry_part.isEnabled() && !part_detected))
                {
                    step = 0;
                }
            break;

            case 20: // aguarda saida de peca no final da esteira
            break;
        }
        vTaskDelay(1);
    }
}


