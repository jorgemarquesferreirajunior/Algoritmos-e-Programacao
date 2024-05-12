#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <i2c.h>
#define PCF8574W  0x4E  // Endere�o i2c do expansor PCF8574 e o opera��o de escrita

// ================================================================= //
// BIBLIOTECA PARA CONTROLE DE DISPLAY LCD VIA I2C		             //
// ================================================================= //
void ControlLCDi2c(unsigned char CMD);
void StartLCDi2c(void);
void WriteLCDi2c(unsigned char DATA);
void StringLCDi2c(unsigned char LINE, rom unsigned char *STR);

// Rotina que envia uma instru��o de inicializa��o ou configura��o para o LCD
void ControlLCDi2c(unsigned char CMD)
{
  unsigned char LCD;

  IdleI2C(); StartI2C();        	// Inicia comunica��o I2C
  IdleI2C(); WriteI2C(PCF8574W); 	// Define dispositivo e opera��o de escrita
  LCD = 0xF0 & CMD;             	// Separa parte alta da instru��o
  LCD = (LCD | 0x0C);           	// Ativa backlight (P3=1), modo instru��o (RS=0) e habilita o display (E=1)
  IdleI2C(); WriteI2C(LCD);     	// Envia instru��o
  Delay10TCYx(1);               	// Aguarda 10us
  LCD = (LCD & 0xF8);           	// Desabilita o display (E=0)
  IdleI2C(); WriteI2C(LCD);     	// Envia instru��o
  LCD = (CMD<<4) & 0xF0;        	// Separa parte baixa da instru��o
  LCD = (LCD | 0x0C);           	// Ativa backlight (P3=1), modo instru��o (RS=0) e habilita o display (E=1)
  IdleI2C(); WriteI2C(LCD);     	// Escreve instru��o
  Delay10TCYx(1);               	// Aguarda 10us
  LCD = (LCD & 0xF8);           	// Desabilita o display (E=0)
  IdleI2C(); WriteI2C(LCD);     	// Envia instru��o
  IdleI2C(); StopI2C();         	// Finaliza comunica��o I2C
  Delay1KTCYx(5);  	            	// Aguarda 5ms
}

// Rotina que realiza a inicializa��o do LCD
void StartLCDi2c(void)
{
  Delay1KTCYx(20);     // Aguarda pelo menos 15ms
  ControlLCDi2c(0x	30); // Instru��o de inicializa��o
  Delay1KTCYx(5);      // Aguarda pelo menos 4.1ms
  ControlLCDi2c(0x30); // Instru��o de inicializa��o
  Delay100TCYx(2);     // Aguarda pelo menos 100us
  ControlLCDi2c(0x30); // Instru��o de inicializa��o
  ControlLCDi2c(0x02); // Define interface de 4 bits
  ControlLCDi2c(0x28); // Define interface de 4 bits e display de 2 linhas
  ControlLCDi2c(0x06); // Modo de entrada: desloca cursor para direita sem deslocar a mensagem
  ControlLCDi2c(0x0C); // Desliga cursor
  ControlLCDi2c(0x01); // Limpa display
}

// Rotina que envia um caractere a ser escrito no LCD
void WriteLCDi2c(unsigned char DATA)
{
  unsigned char LCD;

	IdleI2C(); StartI2C();        	// Inicia comunica��o I2C
	IdleI2C(); WriteI2C(PCF8574W); 	// Define dispositivo e opera��o de escrita
	LCD = 0xF0 & DATA;            	// Separa parte alta do dado
	LCD = LCD | 0x0D;             	// Ativa backlight (P3=1), modo dados (RS=1) e habilita o display (E=1)
	IdleI2C(); WriteI2C(LCD);     	// Envia dado
	Delay10TCYx(10);              	// Aguarda 10us
	LCD = (LCD & 0xF9);           	// Desabilita o display (E=0)
	IdleI2C(); WriteI2C(LCD);     	// Envia dado
	LCD = (DATA<<4) & 0xF0;      		// Separa parte baixa do dado
	LCD = LCD | 0x0D;             	// Ativa backlight (P3=1), modo dados (RS=1) e habilita o display (E=1)
	IdleI2C(); WriteI2C(LCD);     	// Envia dado
	Delay10TCYx(10);              	// Aguarda 10us
	LCD = (LCD & 0xF9);           	// Desabilita o display (E=0)
	IdleI2C(); WriteI2C(LCD);     	// Envia dado
	IdleI2C(); StopI2C();         	// Para comunica��o I2C
	Delay1KTCYx(5);  	            	// Aguarda 5ms
}

// Rotina que escreve uma linha completa no LCD
void StringLCDi2c(unsigned char LINE, rom unsigned char *STR)
{
  if(LINE == 1) ControlLCDi2c(0x80);
  if(LINE == 2) ControlLCDi2c(0xC0);

  // Escreve todos os caracteres at� encontrar o final do ponteiro
  while(*STR != '\0')
  {
    WriteLCDi2c(*STR);
    ++STR;
  }
}

#endif //LCD_I2C_H