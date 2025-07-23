#include "EmonLib.h"
#define an_readpin A0
#define voltage 220
EnergyMonitor ac_amper;
int power = 0;
unsigned long start_time = 0, current_time = 0;
double irms = 0;

void showInfo(void);
void setup() {
  Serial.begin(9600);
  ac_amper.current(an_readpin, 2000 / 330);

  Serial.print("Inicializando...");
  delay(1000);
  Serial.print(" 3 ");
  delay(1000);
  Serial.print(" 2 ");
  delay(1000);
  Serial.print(" 1\n");
  start_time = millis();
}

void loop() {
  irms = ac_amper.calcIrms(1480);
  power = irms*voltage;

  showInfo();

}

void showInfo(void) {
  current_time = millis();
  if(current_time - start_time >= 1000) {
    Serial.print("Current: ");
    Serial.print(irms);
    Serial.println(" A");

    Serial.print("Power: ");
    Serial.print(power);
    Serial.println(" W");
    start_time = current_time;
  }
}

