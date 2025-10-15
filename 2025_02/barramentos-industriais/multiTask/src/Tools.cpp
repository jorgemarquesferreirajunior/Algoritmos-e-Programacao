#include "Tools.hpp"
#include "esp32-hal-gpio.h"

bool prev_stateR = false;
bool prev_stateF = false;

bool r_trig(int pin)
{
    bool actual_stateR = digitalRead(pin);
    bool rising_edge = (!prev_stateR && actual_stateR);
    prev_stateR = actual_stateR;   
    return rising_edge;
}

bool f_trig(int pin)
{
    bool actual_stateF = digitalRead(pin);
    bool falling_edge = (prev_stateF && !   actual_stateF);
    prev_stateF = actual_stateF;   
    return falling_edge;
}
