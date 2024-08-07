//***************************************Includes***************************************//
#include <p18F4550.h>
#include <delays.h>
#include "lcd_i2c.h"
//#include "rtc.h"

//****************************************pragma****************************************//
#pragma config FOSC		= HS
#pragma config PWRT 	= ON
#pragma config WDT 		= OFF
#pragma config MCLRE	= OFF
#pragma config LVP 		= OFF
#pragma interrupt interrupcaoRelogio // Fun��o de tratamento de interrup��es - interrupcao

//--------------------------------mapeamento-de-hardware--------------------------------//
#define LED_BTN_LEFT 	PORTBbits.RB2 
#define LED_BTN_UP		PORTBbits.RB3 
#define LED_BTN_RIGHT 	PORTBbits.RB4 
#define LED_BTN_DOWN 	PORTBbits.RB5 
#define LED_BTN_ENTER 	PORTBbits.RB6

#define FIM_CURSO_TB_SUPERIOR	PORTDbits.RD0
#define FIM_CURSO_TB_INFERIOR	PORTDbits.RD1
#define FIM_CURSO_GV_ABERTO		PORTDbits.RD2
#define FIM_CURSO_GV_FECHADO	PORTDbits.RD3

// Defini��es de bits do registrador T1CON
#define TMR1ON T1CONbits.TMR1ON
#define T1OSCEN T1CONbits.T1OSCEN
#define T1SYNC T1CONbits.T1SYNC
#define TMR1CS T1CONbits.TMR1CS
#define T1CKPS T1CONbits.T1CKPS

// Defini��es de bits do registrador PIE1
#define TMR1IE PIE1bits.TMR1IE

// Defini��es de bits do registrador PIR1
#define TMR1IF PIR1bits.TMR1IF

//--------------------------------------diretivas--------------------------------------//
#define QTD_BTNS 		5
#define VARIACAO		5
//--------------------------------------variaveis--------------------------------------//
unsigned char estado_anterior_btn[QTD_BTNS]	= {0}, flag_btn[QTD_BTNS] = {0}, cursor_pos_x = 0, cursor_pos_y = 0, cursor_visivel = 1, indice = 0, status[2] = {"ON", "OFF"};
unsigned short VAN0, sprays_capacidade = 500, sprays_utilizado = 18;
unsigned short faixas[QTD_BTNS] = {767, 682, 512, 731, 614};// enter,left,up,right,down
enum {enter=0, left, up, right, down};
char secs = 0x39, mins = 0x3B, hours = 0x17, dias = 0x00, relogio[14] = "00h00min00sec";
//****************************************testes****************************************//
unsigned short variavel_teste = 0;
//--------------------------------------prototipos--------------------------------------//

void maskInit(void);
void maskSensores(void);
void maskMotores(void);
void maskPurificador(void);
void maskRelogio(void);
void paginaHome(void);
void paginaConfiguracoes(void);
void paginaInfo(void);
void paginaMe(void);

void bateriaLow();
void cat1();
void cat2();
void loadCaracteres(void);

void atualizaIndice(void);
void atualizaIndiceInfo(void);
void atualizaIndiceConfiguracoes(void);
void menuSelect(void);
void infoSelect(void);
void configuracoesSelect(void);
void TelaInicializacao(void);
void atualizaSensores(void);
void atualizaMotores(void);
void atualizaMotores(void);
void piscaCursor(void);

signed short lerAnalogico(char CANAL);
void lerBotoes(void);
void atualizaBotoes(void);

unsigned char lenShort(unsigned short numero);
void convertShortToChar(unsigned short num, char *str) ;

void delay_ms (unsigned int tempo);

// Prot�tipos das Rotinas de Tratamento de Interrup��es (interrupcaoRelogio)
void configTimer0(void);
void setTimer0(void);
void configInterrupcaoRelogio(void);
void interrupcaoRelogio(void);
void atualizaRelogio(void);
void mostrarRelogio(void);
void setupRelogio(void);

// Redirecionamento para a interrupcaoRelogio
#pragma code high_vector=0x08 // alta prioridade
void interrupt_at_high_vector(void)
{
  _asm GOTO interrupcaoRelogio _endasm
}
#pragma code

//===================================Funcao-Principal===================================//
void main()
{
	PORTA = 0x00; LATA = 0x00; TRISA = 0x01; 
	PORTB = 0x00; LATB = 0x00; TRISB = 0x03; 
	PORTC = 0x00; LATC = 0x00; TRISC = 0x00; 
	PORTD = 0x00; LATD = 0x00; TRISD = 0x0F; 
	PORTE = 0x00; LATE = 0x00; TRISE = 0x00;
	
	ADCON2 = 0x81;
	ADCON1 = 0x0E;
	ADCON0 = 0x01;
	CMCON = 0x07;
	
	configTimer0();
	configInterrupcaoRelogio();

	OpenI2C(MASTER, SLEW_OFF); 	// Inicializa I2C: Mestre com velocidade de 100Khz.
	SSPADD = 9;                	// Taxa de comunica��o I2C de 100khz
	
	StartLCDi2c();	
	maskInit();
	
	while(1)
	{
		TelaInicializacao();
	}
}
//----------------------------------------telas----------------------------------------//


void maskInit(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("  HOME   CONFIG      ");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("  INFO   ABOUT-ME     ");
}

void maskSensores(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("TAMBOR RECUADO ");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("TAMBOR AVANCADO");
	setCursorLCD_i2c(2, 0);printStringLCD_i2c("GAVETA ABERTO  ");
	setCursorLCD_i2c(3, 0);printStringLCD_i2c("GAVETA FECHADO ");
}

void maskPurificador(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("----PURIFICADOR---- ");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("SPRAYS 000/000      ");
}

void maskMotores(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("MOTOR TAMBOR ");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("MOTOR GAVETA ");
}

void maskRelogio(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("HORA....:           ");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("MINUTO..:           ");
	setCursorLCD_i2c(2, 0);printStringLCD_i2c("SEGUNDO.:           ");
}

// SUB-ROTINA TELA paginaHome
void paginaHome(void)
{
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("------CAT-BOX------");
	
	atualizaBotoes();
	while(!flag_btn[left])
	{				
		atualizaRelogio();mostrarRelogio();
		atualizaBotoes();		
	}
	flag_btn[left] = 0;
	cursor_pos_x = cursor_pos_y = 0;// cursor home	
	atualizaIndice();
	maskInit();TelaInicializacao();
}

// SUB-ROTINA TELA paginaConfiguracoes
void paginaConfiguracoes(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	printStringLCD_i2c("=-=-=-AJUSTES-=-=-=");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("  Motores");
	setCursorLCD_i2c(2, 0);printStringLCD_i2c("  Sensores");
	setCursorLCD_i2c(3, 0);printStringLCD_i2c("  Relogio");
	cursor_pos_y = 1;cursor_pos_x = 0;
	
	atualizaBotoes();
	while(!flag_btn[left])
	{		
		if(flag_btn[up])
		{
			flag_btn[up] = 0;
			LED_BTN_UP = !LED_BTN_UP;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 1)
			{
				cursor_pos_y = 3;
			}else
			{
				cursor_pos_y --;
			}	
		}
		if(flag_btn[down])
		{
			flag_btn[down] = 0;
			LED_BTN_DOWN = !LED_BTN_DOWN;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 3)
			{
				cursor_pos_y = 1;
			}else
			{
				cursor_pos_y ++;
			}		
		}
		atualizaBotoes();
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);piscaCursor();
		atualizaIndiceConfiguracoes();configuracoesSelect();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 0;cursor_pos_x = 8; // cursor configuracoes
	atualizaIndice();
	maskInit();TelaInicializacao();
	
}

// SUB-ROTINA TELA paginaInfo
void paginaInfo(void)
{
	cmdLCD_i2c(_LCD_LIMPA);
	printStringLCD_i2c("---CAT-BOX V: 1.0---");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("  Motores           ");
	setCursorLCD_i2c(2, 0);printStringLCD_i2c("  Sensores          ");
	setCursorLCD_i2c(3, 0);printStringLCD_i2c("  Purificador       ");
	cursor_pos_y = 1;cursor_pos_x = 0;
	
	atualizaBotoes();
	while(!flag_btn[left])
	{		
		if(flag_btn[up])
		{
			flag_btn[up] = 0;
			LED_BTN_UP = !LED_BTN_UP;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 1)	cursor_pos_y = 3;
			else					cursor_pos_y --;	
		}
		if(flag_btn[down])
		{
			flag_btn[down] = 0;
			LED_BTN_DOWN = !LED_BTN_DOWN;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 3)	cursor_pos_y = 1;
			else					cursor_pos_y ++;		
		}
		atualizaBotoes();
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);piscaCursor();
		atualizaIndiceInfo();infoSelect();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 1;cursor_pos_x = 0;// cursor info
	atualizaIndice();
	maskInit();TelaInicializacao();
}

// SUB-ROTINA TELA paginaMe
void paginaMe(void)
{
	setCursorLCD_i2c(0, 0);printStringLCD_i2c("Developed by:");
	setCursorLCD_i2c(1, 0);printStringLCD_i2c("Jorge M. F. J.");
	
	atualizaBotoes();
	while(!flag_btn[left])
	{
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 1;cursor_pos_x = 8; // cursor me
	atualizaIndice();
	maskInit();TelaInicializacao();
}


//---------------------------------caracteres-especiais---------------------------------//
// SUB-ROTINA PARA CRIACAO DE CARACTERE ESPECIAL NO DISPLAY	
void bateriaLow()
{
	cmdLCD_i2c(_LCD_CMD);
	printCharLCD_i2c(0b00000100);
	printCharLCD_i2c(0b00011011);
	printCharLCD_i2c(0b00010001);
	printCharLCD_i2c(0b00010001);
	printCharLCD_i2c(0b00010001);
	printCharLCD_i2c(0b00010001);
	printCharLCD_i2c(0b00011111);
	printCharLCD_i2c(0b00011111);
}

// SUB-ROTINA PARA CRIACAO DE CARACTERE ESPECIAL NO DISPLAY	
void cat1()
{
	cmdLCD_i2c(_LCD_CMD);
	printCharLCD_i2c(0b00000100);
	printCharLCD_i2c(0b00001110);
	printCharLCD_i2c(0b00000000);
	printCharLCD_i2c(0b00000000);
	printCharLCD_i2c(0b00000001);
	printCharLCD_i2c(0b00010011);
	printCharLCD_i2c(0b00011110);
	printCharLCD_i2c(0b00000000);
}

// SUB-ROTINA PARA CRIACAO DE CARACTERE ESPECIAL NO DISPLAY	
void cat2()
{
	cmdLCD_i2c(_LCD_CMD);
	printCharLCD_i2c(0b00000100);
	printCharLCD_i2c(0b00001110);
	printCharLCD_i2c(0b00000000);
	printCharLCD_i2c(0b00000000);
	printCharLCD_i2c(0b00010000);
	printCharLCD_i2c(0b00011001);
	printCharLCD_i2c(0b00001111);
	printCharLCD_i2c(0b00000000);
}

// SUB-ROTINA P/ SALVAR CARACTERES ESPECIAIS NA MEMORIA DO DISPLAY
void loadCaracteres(void)
{
	cat1();
	cat2();
	bateriaLow();
}

//------------------------------------controle-telas------------------------------------//
// SUB-ROTINA P/ ATUALIZAR O INDICE DO DISPLAY
void piscaCursor(void)
{	
	if(cursor_visivel)	printCharLCD_i2c('>');
	else				printCharLCD_i2c('  ');
}

void atualizaIndice(void)
{
	if(cursor_pos_y == 0 && cursor_pos_x == 0) 		indice = 1; //home
	else if(cursor_pos_y == 0 && cursor_pos_x == 8) indice = 2; // config
	else if(cursor_pos_y == 1 && cursor_pos_x == 0) indice = 3; // info
	else if(cursor_pos_y == 1 && cursor_pos_x == 8) indice = 4; // me
}

void atualizaIndiceInfo(void)
{
	if(cursor_pos_y == 1)		indice = 1; // motores
	else if(cursor_pos_y == 2) 	indice = 2; // sensores
	else if(cursor_pos_y == 3) 	indice = 3; // purificador
}

void atualizaIndiceConfiguracoes(void)
{
	if(cursor_pos_y == 1)		indice = 1; // motores
	else if(cursor_pos_y == 2)	indice = 2; // sensores
	else if(cursor_pos_y == 3)	indice = 3; // relogio
}

void atualizaSensores(void)
{
	atualizaBotoes();
	
	while(!flag_btn[left])
	{
		flag_btn[left] = 0;
		setCursorLCD_i2c(0, 16);
		if(FIM_CURSO_TB_SUPERIOR)	printStringLCD_i2c("ON ");
		else 						printStringLCD_i2c("OFF");
		
		setCursorLCD_i2c(1, 16);
		if(FIM_CURSO_TB_INFERIOR)	printStringLCD_i2c("ON ");
		else						printStringLCD_i2c("OFF");

		setCursorLCD_i2c(2, 16);
		if(FIM_CURSO_GV_ABERTO)		printStringLCD_i2c("ON ");
		else						printStringLCD_i2c("OFF");
		
		setCursorLCD_i2c(3, 16);
		if(FIM_CURSO_GV_FECHADO)	printStringLCD_i2c("ON ");	
		else						printStringLCD_i2c("OFF");
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 2; cursor_pos_x = 0;
	atualizaIndiceInfo();paginaInfo();
}

void atualizaMotores(void)
{
	atualizaBotoes();
	
	while(!flag_btn[left])
	{
		// implementar
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 1; cursor_pos_x = 0;
	atualizaIndiceInfo();paginaInfo();
}

void mostraPurificador(void)
{
	atualizaBotoes();
	
	while(!flag_btn[left])
	{
		setCursorLCD_i2c(1,13);printShortLCD_i2c(sprays_capacidade, lenShort(sprays_capacidade));
		setCursorLCD_i2c(1,9);printShortLCD_i2c(sprays_utilizado, lenShort(sprays_utilizado));
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 3; cursor_pos_x = 0;
	atualizaIndiceInfo();paginaInfo();
}

// SUB-ROTINA PARA SELECAO DO MENU DE VISUALIZACAO
void menuSelect(void)
{
	if(flag_btn[enter])
	{
		flag_btn[enter] = 0;
		LED_BTN_ENTER = !LED_BTN_ENTER;	
		cmdLCD_i2c(_LCD_LIMPA);
		switch(indice)
		{
			case 1:
				paginaHome();
				break;
			case 2:
				paginaConfiguracoes();
				break;
			case 3:
				paginaInfo();
				break;
			case 4:
				paginaMe();
				break;
		}
	}
}

void infoSelect(void)
{
	if(flag_btn[enter])
	{
		flag_btn[enter] = 0;
		LED_BTN_ENTER = !LED_BTN_ENTER;	
		cmdLCD_i2c(_LCD_LIMPA);
		switch(indice)
		{
			case 1:
				maskMotores();
				atualizaMotores();
				break;
			case 2:
				maskSensores();
				atualizaSensores();
				break;
			case 3:
				maskPurificador();
				mostraPurificador();
				break;
		}
	}
	if(flag_btn[left])
	{
		flag_btn[left] = 0;
		cursor_pos_y = 1;cursor_pos_x = 0;
		atualizaIndice();
		maskInit();TelaInicializacao();
	}
}

void configuracoesSelect(void)
{
	if(flag_btn[enter])
	{
		flag_btn[enter] = 0;
		LED_BTN_ENTER = !LED_BTN_ENTER;	
		cmdLCD_i2c(_LCD_LIMPA);
		switch(indice)
		{
			case 1:
				maskRelogio();
				setupRelogio();
				break;
			case 2:
				maskRelogio();
				setupRelogio();
				break;
			case 3:
				maskRelogio();
				setupRelogio();
				break;
		}
	}
}
// SUB-ROTINA PARA NAVEGACAO NA TELA DE INICIALIZACAO
void TelaInicializacao(void)
{	
	setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
	atualizaIndice();
	atualizaBotoes();
	if(flag_btn[left])
	{
		flag_btn[left] = 0;
		LED_BTN_LEFT = !LED_BTN_LEFT;
		if(cursor_pos_x > 0)
		{
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
			printCharLCD_i2c('  ');
			cursor_pos_x -= 8;
		}				
	}
	if(flag_btn[up])
	{
		flag_btn[up] = 0;
		LED_BTN_UP = !LED_BTN_UP;
		if(cursor_pos_y > 0)
		{
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			cursor_pos_y --;
		}				
	}
	if(flag_btn[right])
	{
		flag_btn[right] = 0;
		LED_BTN_RIGHT = !LED_BTN_RIGHT;
		if(cursor_pos_x < 8)
		{
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			cursor_pos_x += 8;
		}			
	}
	if(flag_btn[down])
	{
		flag_btn[down] = 0;
		LED_BTN_DOWN = !LED_BTN_DOWN;
		if(cursor_pos_y < 1)
		{
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			cursor_pos_y ++;
		}			
	}
	setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);piscaCursor();
	atualizaIndice();menuSelect();
}

//------------------------------controle-botoes-analogicos------------------------------//
// SUB-ROTINA PARA LEITURA DE UMA PORTA ANALOGICA DO PIC
signed short lerAnalogico(char CANAL)
{
	ADCON0bits.CHS = CANAL;		// Seleciona canal A/D
	Delay100TCYx(1);				// Aguarda tempo para troca de canal antes de iniciar convers�o
	ADCON0bits.GO_DONE = 1;    	// Inicia a convers�o
	while(ADCON0bits.GO_DONE);  // Aguarda t�rmino da convers�o
	
	return ADRES;
}

// SUB-ROTINA PARA MONITORAMENTO DOS BOTOES ANALOGICOS
void lerBotoes(void)
{
	unsigned char i;	
	for(i = 0; i < QTD_BTNS; i++)
	{
		unsigned char estado_atual = 0;
		
		if((faixas[i] - VARIACAO)<= VAN0 && (faixas[i] + VARIACAO)>= VAN0)
		{
			estado_atual = 1;
			
			if(estado_atual && !estado_anterior_btn[i])	flag_btn[i]= 1;
		}
		estado_anterior_btn[i] = estado_atual;
	}
}

void atualizaBotoes(void)
{
	VAN0 = lerAnalogico(0);
	lerBotoes();
}
//---------------------------------manipulacao-de-dados---------------------------------//
// SUB-ROTINA PARA RETORNAR A QUANTIDADE DE DIGITOS DE UM NUMERO
unsigned char lenShort(unsigned short numero)
{
	int temp = numero;
	unsigned char qtd = 0;
	
	do
	{
		temp /= 10;
		qtd ++;
	}while(temp != 0);
	
	return qtd;
}

// SUB-ROTINA PARA CONVERSAO DE UM INT PARA CHAR
void convertShortToChar(unsigned short num, char *str) 
{
	
	int length = lenShort(num); // Come�amos com um d�gito para o caractere nulo
	int i = 0;
		
	// Converte o n�mero em uma string de caracteres
	for (i = length - 1; i >= 0; i--)
	{
		str[i] = num % 10 + '0'; // Converte o d�gito para seu caractere correspondente
		num /= 10;
	}
	str[length] = '\0'; // Adiciona o caractere nulo ao final da string
}
//--------------------------------funcao-auxiliar-delay--------------------------------//
void delay_ms (unsigned int tempo)
{
	while(tempo > 0){
		Delay100TCYx(10);
		tempo--;
	}
}

void interrupcaoRelogio(void)
{
   if(INTCONbits.TMR0IF == 1)
   {
      secs++;
      cursor_visivel = !cursor_visivel;
      if(secs == 0x3c)
      {
	      secs = 0x00;
	      mins++;
	      if(mins == 0x3c)
	      {
		      mins = 0x00;
		      hours++;
		      if(hours == 0x18) hours = 0x00;
	      }
      }
      setTimer0();
   }
}

void configInterrupcaoRelogio(void)
{
   // Configura��o das interrup��es	
   RCONbits.IPEN 		= 0; // Desabilita atribui��o de prioridades �s interrup��es
   INTCONbits.GIE 		= 1; // Habilita todas as interrup��es
   INTCONbits.PEIE 		= 0; // Desabilita interrup��es geradas a partir de perif�ricos do PIC
   INTCONbits.TMR0IE 	= 1; // Habilita interrup��o do Timer0
}

void configTimer0(void)
{
  T0CONbits.TMR0ON 	= 1; //1 = habilita o contador 0 = para o contador
  T0CONbits.T08BIT 	= 0; //1 = contador de 8 bits (0 a 255) 0 = contador de 16 bits (0 a 65.535)
  T0CONbits.T0CS 	= 0; //1 = fonte de clock externa pelo pino RA4/T0CKI 0 = fonte de clock do oscilador interno do microcontrolador
  T0CONbits.T0SE 	= 0; //[Ignorado se T0CS = 0] 1 = incremento na borda de descida do pino T0CKI 0 = incremento na borda de subida do pino T0CKI	
  T0CONbits.PSA 	= 0; //1 = PRESCALER n�o utilizado 0 = PRESCALER habilitado de acordo com a configura��o dos bits T0PS2:T0PS0	
  // [Ignorado se PSA = 1]. COMBINACOES:
  // 111 = 1:256     110 = 1:128     101 = 1:64     100 = 1:32     011 = 1:16     010 = 1:8     001 = 1:4     000 = 1:2
  T0CONbits.T0PS2 	= 0;
  T0CONbits.T0PS1 	= 1;
  T0CONbits.T0PS0 	= 1;
  setTimer0();
}

void setTimer0(void)
{
   INTCONbits.TMR0IF = 0;
   TMR0H = 0x0B;          
   TMR0L = 0xDC;
}

void atualizaRelogio(void)
{
	relogio[1] = hours%10 + '0';
	relogio[0] = hours/10 + '0';
	
	relogio[4] = mins%10 + '0';
	relogio[3] = mins/10 + '0';
	
	relogio[9] = secs%10 + '0';
	relogio[8] = secs/10 + '0';
}

void mostrarRelogio(void)
{
	unsigned char i = 0;
	setCursorLCD_i2c(1,4);
	
	for(i=0;i<14;i++) printCharLCD_i2c(relogio[i]);
}

void setupRelogio(void)
{
	
	INTCONbits.TMR0IE 	= !INTCONbits.TMR0IE; // Desabilita interrup��o do Timer0
	while(!flag_btn[left])
	{
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	paginaConfiguracoes();
}