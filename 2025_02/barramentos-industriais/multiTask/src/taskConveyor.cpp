#include <Arduino.h>
#include "Globals.hpp"
#include "MyTasks.hpp"
#include "Btn.hpp"
#include "Ton.hpp"
#include "esp32-hal-gpio.h"

int last_cont = 0;
int step = 0;
unsigned long timeout_part_type = 5000; // tempo limite para deteccao de algum tipo de peca
TON ton00(500);
TON ton01(5000);

Btn btn_start_process(INIT_PROCESS, true);
Btn btn_part_in_position(PART_AT_ENTRY, true);
Btn btn_part_in_exit(PART_AT_EXIT, true);
Btn btn_is_part_type1(PART_TYPE1, true);
Btn btn_is_part_type2(PART_TYPE2, true);
Btn btn_is_part_type3(PART_TYPE3, true);


void taskConveyor(void *pvParameters)
{
    ton00.reset();
    ton01.reset();

    while (true)
    {
        if (btn_start_process.press())
        {
            switch (step) {
                case 0:
                    if(ton00.update(!ton00.isEnabled() && !btn_part_in_position.press()))
                    {
                        digitalWrite(STATUS_MACHINE, !digitalRead(STATUS_MACHINE));
                    }

                    if (btn_part_in_position.press()) {
                        step = 10;
                        // ligar motor

                    }
                    break;
                case 10:
                    ton00.reset();
                    bool part_detected = btn_is_part_type1.press() || btn_is_part_type2.press() || btn_is_part_type3.press();

                    if(ton01.update(!ton01.isEnabled() && !part_detected))
                    {
                        step = 0;
                    }
                    else if () {
                    statements
                    }


                    break;
                default:
                    break;
            }
        } 
        vTaskDelay(1);
    }
}


