#include "EmonLib.h"
#include "LiquidCrystal_I2C.h"
#include "myButtons.h"

#define an_readpin A9
#define voltage 220
#define MAX_CHAR 10
EnergyMonitor ac_amper;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

int n_samples = 1480;
unsigned long start_time = 0;
double irms = 0.0, power = 0.0, epsilon = 0.0;
bool flag_info = false;
char irms_str[MAX_CHAR], power_str[MAX_CHAR];

// Functions
void showInfo(void);
void initLCD(void);
void updtMeasures(void);
void updtDisplay(void);
void formatValues(void);

void setup() {
  Serial.begin(9600);
  ac_amper.current(an_readpin, 2000 / 330);
  initLCD();

  start_time = millis();
}

void loop() {
  if (millis() - start_time >= 1000) {
    updtMeasures();
    formatValues();
    start_time = millis();
    lcd.clear();
  }
  Serial.println(flag_info);
  updtDisplay();
}


// Functions

void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" INITIALIZING !");
  lcd.setCursor(0, 1);
  delay(1500);
  lcd.clear();
}

void updtMeasures() {
  double new_irms = ac_amper.calcIrms(n_samples);
  if (new_irms <= 0.06) new_irms = 0.0;
  double new_power = new_irms * voltage;

  if (abs(new_irms - irms) > epsilon) irms = new_irms;
  if (abs(new_power - power) > epsilon) power = new_power;
}

void formatValues() {
  if (irms <= 0.01) irms = 0.0;
  if (power <= 0.06) power = 0.0;

  irms *= 1000;

  dtostrf(irms, MAX_CHAR-2, 2, irms_str);
  dtostrf(power, MAX_CHAR-2, 2, power_str);

  for (int i = 0; i < MAX_CHAR-2; i++) {
    if (irms_str[i] == ' ') irms_str[i] = '0';
    if (power_str[i] == ' ') power_str[i] = '0';
  }
}

void updtDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("I:");
  lcd.print(irms_str);
  lcd.setCursor(11, 0);
  lcd.print("mA");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(power_str);
  lcd.setCursor(11, 1);
  lcd.print("Watts");
}
