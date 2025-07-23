#include "EmonLib.h"
#include "ball.h"
#include "Arduino_ST7789_Fast.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#define an_readpin A0
#define voltage 220

#define TFT_DC 20
#define TFT_RST 19

#define BUTTON 3
#define SCR_WD 240
#define SCR_HT 240

#define LINE_YS 20
#define LINE_XS1 30
#define LINE_XS2 6
#define BALL_WD 64
#define BALL_HT 64
#define BALL_SWD 240
#define BALL_SHT 180
#define SP 20
#define SHADOW 20

EnergyMonitor ac_amper;
int power = 0;
unsigned long start_time = 0, current_time = 0;
double irms = 0;

Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);
uint16_t palette[16];
uint16_t line[SCR_WD];
uint16_t bgCol = RGBto565(160, 160, 160);
uint16_t bgColS = RGBto565(100, 100, 100);
uint16_t lineCol = RGBto565(150, 40, 150);
uint16_t lineColS = RGBto565(90, 20, 90);
int anim = 0, animd = 2;
int x = 0, y = 0;
int xd = 4, yd = 1;
unsigned long ms;

void showInfo(void);
void setupDisplay(void);
void drawBall(int x, int y);
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
  power = irms * voltage;

  setupDisplay();

  showInfo();

  ms=millis();
  for(int i=0;i<14;i++) {
    palette[i+1] = ((i+anim)%14)<7 ? WHITE : RED;
    //int c=((i+anim)%14); // with pink between white and red
    //if(c<6) palette[i+1]=WHITE; else if(c==6 || c==13) palette[i+1]=RGBto565(255,128,128); else palette[i+1]=RED;
  }
  drawBall(x,y);
  anim+=animd;
  if(anim<0) anim+=14;
  x+=xd;
  y+=yd;
  if(x<0) { x=0; xd=-xd; animd=-animd; }
  if(x>=BALL_SWD-BALL_WD) { x=BALL_SWD-BALL_WD; xd=-xd; animd=-animd; }
  if(y<0) { y=0; yd=-yd; }
  if(y>=BALL_SHT-BALL_HT) { y=BALL_SHT-BALL_HT; yd=-yd; }
  //ms=millis()-ms; Serial.println(ms);
}

void showInfo(void) {
  current_time = millis();
  if (current_time - start_time >= 1000) {
    Serial.print("Current: ");
    Serial.print(irms);
    Serial.println(" A");

    Serial.print("Power: ");
    Serial.print(power);
    Serial.println(" W");
    start_time = current_time;
  }
}


void setupDisplay(void) {
  Serial.begin(115200);
  lcd.init();
  lcd.fillScreen(bgCol);
  int i, o;
  uint16_t *pal = (uint16_t *)ball + 3;
  for (i = 0; i < 16; i++) palette[i] = pgm_read_word(&pal[i]);
  for (i = 0; i < 10; i++) lcd.drawFastVLine(LINE_XS1 + i * SP, LINE_YS, 8 * SP, lineCol);
  for (i = 0; i < 9; i++) lcd.drawFastHLine(LINE_XS1, LINE_YS + i * SP, SCR_WD - LINE_XS1 * 2, lineCol);
  lcd.drawFastHLine(LINE_XS2, SCR_HT - LINE_YS, SCR_WD - LINE_XS2 * 2, lineCol);
  int dy = SCR_HT - LINE_YS - (LINE_YS + SP * 8);
  int dx = LINE_XS1 - LINE_XS2;
  o = 2 * 7 * dx / dy;
  lcd.drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - 7 * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  o = 2 * (7 + 6) * dx / dy;
  lcd.drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - (7 + 6) * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  o = 2 * (7 + 6 + 4) * dx / dy;
  lcd.drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - (7 + 6 + 4) * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  for (i = 0; i < 10; i++) lcd.drawLine(LINE_XS1 + i * SP, LINE_YS + SP * 8, LINE_XS2 + i * (SCR_WD - LINE_XS2 * 2) / 9, SCR_HT - LINE_YS, lineCol);
  //delay(10000);
}

void drawBall(int x, int y) {
  int i, j, ii;
  for (j = 0; j < BALL_HT; j++) {
    uint8_t v, *img = (uint8_t *)ball + 16 * 2 + 6 + j * BALL_WD / 2 + BALL_WD / 2;
    int yy = y + j;
    if (yy == LINE_YS || yy == LINE_YS + 1 * SP || yy == LINE_YS + 2 * SP || yy == LINE_YS + 3 * SP || yy == LINE_YS + 4 * SP || yy == LINE_YS + 5 * SP || yy == LINE_YS + 6 * SP || yy == LINE_YS + 7 * SP) {  // ugly but fast
                                                                                                                                                                                                                //if(((yy-LINE_YS)%SP)==0) {
      for (i = 0; i < LINE_XS1; i++) line[i] = line[SCR_WD - 1 - i] = bgCol;
      for (i = 0; i <= SCR_WD - LINE_XS1 * 2; i++) line[i + LINE_XS1] = lineCol;
    } else {
      for (i = 0; i < SCR_WD; i++) line[i] = bgCol;
      if (yy > LINE_YS)
        for (i = 0; i < 10; i++) line[LINE_XS1 + i * SP] = lineCol;
    }
    for (i = BALL_WD - 2; i >= 0; i -= 2) {
      v = pgm_read_byte(--img);
      if (v >> 4) {
        line[x + i + 0] = palette[v >> 4];
#if SHADOW > 0
        ii = x + i + 0 + SHADOW;
        if (ii < SCR_WD) {
          if (line[ii] == bgCol) line[ii] = bgColS;
          else if (line[ii] == lineCol) line[ii] = lineColS;
        }
#endif
      }
      if (v & 0xf) {
        line[x + i + 1] = palette[v & 0xf];
#if SHADOW > 0
        ii = x + i + 1 + SHADOW;
        if (ii < SCR_WD) {
          if (line[ii] == bgCol) line[ii] = bgColS;
          else if (line[ii] == lineCol) line[ii] = lineColS;
        }
#endif
      }
    }
    lcd.drawImage(0, yy, SCR_WD, 1, line);
  }
}
