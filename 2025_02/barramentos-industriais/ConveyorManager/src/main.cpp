#include <Arduino.h>
#include "Btn.hpp"


/*
 * CONTROLAR DA ESTEIRA
 *
 * COMUNICACAO
 * 1. ESCRAVO MODBUS RTU (CONTROLADOR <-> PC)
 * 2. MESTRE MODBUS RTU (CONTROLADOR <-> SENSOR DE VISAO)
 *
 * INPUTS
 * (digitais)
 * 1. presenca de peca na etrada da esteira
 * 2. presenca de peca na saida da esteira
 * 3. encoder motor esteira
 * OUTPUTS
 * (digitais)
 * 1. led verde
 * 2. led amarelo
 * 3. led vermelho
 
 * */

#define IN1 22
#define IN2 23
#define DIR 5
#define ENB 18
#define INC 19
#define DEC 21

Btn btn_dir(DIR, false);
Btn btn_enb(ENB, false);
Btn btn_inc(INC, false);
Btn btn_dec(DEC, false);

bool motor_enb = false;
bool motor_dir = false;

void setDir(bool eanable, bool direction);
unsigned long start_time = 0;
void setup()
{
    Serial.begin(9600);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(DIR, INPUT_PULLUP);
    pinMode(ENB, INPUT_PULLUP);
    pinMode(INC, INPUT_PULLUP);
    pinMode(DEC, INPUT_PULLUP);
}

void loop()
{
    unsigned long current_time= millis();
    
    if (btn_enb.press()) motor_enb = ! motor_enb;
    if (btn_dir.press()) motor_dir = !motor_dir;
    
    setDir(motor_enb, motor_dir);
   
    if (current_time - start_time >= 1000)
    {
        start_time = current_time;
        Serial.print("Dir: ");
        Serial.println(motor_dir);
        Serial.print("Enb: ");
        Serial.println(motor_enb);
        Serial.println("");
    }
}


void setDir(bool enable, bool direction)
{
    
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    if (!enable) return;
    direction ? digitalWrite(IN1, HIGH) : digitalWrite(IN2, HIGH); 
}
