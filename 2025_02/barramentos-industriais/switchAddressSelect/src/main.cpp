#include "HardwareSerial.h"
#include <Arduino.h>

const int switchSelect0 = 18;
const int switchSelect1 = 19;
const int switchSelect2 = 21;
byte slaveAddress = 0x00;

unsigned long startT;

void setup()
{
    Serial.begin(9600, SERIAL_8N1);
    pinMode(switchSelect0, INPUT_PULLDOWN);
    pinMode(switchSelect1, INPUT_PULLDOWN);
    pinMode(switchSelect2, INPUT_PULLDOWN);
    startT = millis();
}

void loop()
{
    slaveAddress &= ~((1 << 0) | (1 << 1) | (1 << 2));

    digitalRead(switchSelect0) ? slaveAddress |= (1 << 0) : slaveAddress = slaveAddress;
    digitalRead(switchSelect1) ? slaveAddress |= (1 << 1) : slaveAddress = slaveAddress;
    digitalRead(switchSelect2) ? slaveAddress |= (1 << 2) : slaveAddress = slaveAddress;

    if (millis() - startT >= 1000) {
        Serial.println(slaveAddress, HEX);
        startT = millis();
    }
}
