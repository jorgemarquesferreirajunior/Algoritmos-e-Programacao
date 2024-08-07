//***************************************Includes***************************************//
#include <delays.h>
#include "lcd_i2c.h"
#include "mascaras.c"

//****************************************pragma****************************************//
#pragma config FOSC		= HS
#pragma config PWRT 	= ON
#pragma config WDT 		= OFF
#pragma config MCLRE	= OFF
#pragma config LVP 		= OFF
#pragma interrupt interrupcaoRelogio // Funcaoo de tratamento de interrupcoes - interrupcao

//--------------------------------mapeamento-de-hardware--------------------------------//
#define LED_BTN_LEFT 			PORTBbits.RB2 
#define LED_BTN_UP				PORTBbits.RB3 
#define LED_BTN_RIGHT 			PORTBbits.RB4 
#define LED_BTN_DOWN 			PORTBbits.RB5 
#define LED_BTN_ENTER 			PORTBbits.RB6
#define FIM_CURSO_TB_SUPERIOR	PORTDbits.RD0
#define FIM_CURSO_TB_INFERIOR	PORTDbits.RD1
#define FIM_CURSO_GV_ABERTO		PORTDbits.RD2
#define FIM_CURSO_GV_FECHADO	PORTDbits.RD3
#define LED_ON					PORTDbits.RD4

//--------------------------------------diretivas--------------------------------------//
#define QTD_BTNS 		5
#define VARIACAO		5
//--------------------------------------variaveis--------------------------------------//
unsigned char estado_anterior_btn[QTD_BTNS]	= {0}, flag_btn[QTD_BTNS] = {0}, cursor_pos_x = 0, cursor_pos_y = 0, cursor_visivel = 1, indice = 0, estado_btn, status[2] = {"ON", "OFF"};
unsigned short VAN0, sprays_capacidade = 500, sprays_utilizado = 18;
unsigned short faixas[QTD_BTNS] = {767, 682, 512, 731, 614};// enter,left,up,right,down
enum {enter=0, left, up, right, down};
char secs = 0x39, mins = 0x3B, hours = 0x17, dias = 0x00, relogio[14] = "00h00min00sec";
//****************************************testes****************************************//
unsigned short variavel_teste = 0;
//--------------------------------------prototipos--------------------------------------//

void paginaHome(void);
void paginaConfiguracoes(void);
void paginaInfo(void);
void paginaMe(void);

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
void lerBtnLeft(void);
void lerBtnRight(void);
void lerBtnUp(void);
void lerBtnDown(void);
void lerBtnEnter(void);
void atualizaBotoes(void);
void resetBotoes(void);

void convertShortToChar(unsigned short num, char *str) ;

void delay_ms (unsigned int tempo);

// Prot?tipos das Rotinas de Tratamento de Interrup??es (interrupcaoRelogio)
void configTimer1(void);
void setTimer1(void);
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

//----------------------------------------telas----------------------------------------//

// SUB-ROTINA PARA NAVEGACAO NA TELA DE INICIALIZACAO
void TelaInicializacao(void)
{	
	setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);

	lerBotoes();

	if(flag_btn[left])
	{
		LED_BTN_LEFT = !LED_BTN_LEFT;
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
		printCharLCD_i2c('  ');
		if(cursor_pos_x == 0) cursor_pos_x = 8;
		else cursor_pos_x -= 8;	
	}
	if(flag_btn[up])
	{
		LED_BTN_UP = !LED_BTN_UP;
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
		printCharLCD_i2c('  ');
		if(cursor_pos_y == 0) cursor_pos_y = 1;
		else cursor_pos_y --;			
	}
	if(flag_btn[right])
	{
		LED_BTN_RIGHT = !LED_BTN_RIGHT;
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
		printCharLCD_i2c('  ');
		if(cursor_pos_x == 8) cursor_pos_x = 0;
		else cursor_pos_x += 8;	
	}
	if(flag_btn[down])
	{
		LED_BTN_DOWN = !LED_BTN_DOWN;
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
		if(cursor_pos_y == 1) cursor_pos_y = 0;
		else cursor_pos_y ++;	
	}
	atualizaIndice();
	setCursorLCD_i2c(3, 19);
	printShortLCD_i2c(indice);
	setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
	piscaCursor();
	resetBotoes();

}

// SUB-ROTINA TELA paginaHome
void paginaHome(void)
{
	setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);
	printStringLCD_i2c("------CAT-BOX------");
	
	while(!flag_btn[left])
	{				
		atualizaRelogio();
		mostrarRelogio();
		lerBtnLeft();
	}
	flag_btn[left] = 0;
	maskInit();
}

// SUB-ROTINA TELA paginaConfiguracoes
void paginaConfiguracoes(void)
{
	setCursorLCD_i2c(0,0);printStringLCD_i2c("=-=-=-AJUSTES-=-=-=");
	setCursorLCD_i2c(1,0);printStringLCD_i2c("  Motores");
	setCursorLCD_i2c(2,0);printStringLCD_i2c("  Sensores");
	setCursorLCD_i2c(3,0);printStringLCD_i2c("  Relogio");
	cursor_pos_y = 1;cursor_pos_x = 0;
	
	while(!flag_btn[left])
	{	
		lerBtnUp();
		lerBtnDown();
		lerBtnLeft();
		if(flag_btn[up])
		{
			flag_btn[up] = 0;
			LED_BTN_UP = !LED_BTN_UP;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 1) cursor_pos_y = 3;
			else cursor_pos_y --;
		}
		if(flag_btn[down])
		{
			flag_btn[down] = 0;
			LED_BTN_DOWN = !LED_BTN_DOWN;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);printCharLCD_i2c('  ');
			if(cursor_pos_y == 3) cursor_pos_y = 1;
			else cursor_pos_y ++;		
		}
		
		atualizaIndiceConfiguracoes();
		setCursorLCD_i2c(3,19);
		printShortLCD_i2c(indice);
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);piscaCursor();
		configuracoesSelect();
	}
	resetBotoes();
	cursor_pos_y = 0;cursor_pos_x = 8; // cursor configuracoes
	maskInit();
	
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

//------------------------------------controle-telas------------------------------------//
// SUB-ROTINA P/ ATUALIZAR O INDICE DO DISPLAY
void piscaCursor(void)
{	
	if(cursor_visivel)	printCharLCD_i2c('>');
	else				printCharLCD_i2c('  ');
}

void atualizaIndice(void)
{
	if (cursor_pos_y == 0 && cursor_pos_x == 0) 	 indice = 1; //home
	else if (cursor_pos_y == 0 && cursor_pos_x == 8) indice = 2; // config
	else if (cursor_pos_y == 1 && cursor_pos_x == 0) indice = 3; // info
	else if (cursor_pos_y == 1 && cursor_pos_x == 8) indice = 4; // me
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
		setCursorLCD_i2c(1,13);printShortLCD_i2c(sprays_capacidade);
		setCursorLCD_i2c(1,9);printShortLCD_i2c(sprays_utilizado);
		atualizaBotoes();
	}
	flag_btn[left] = 0;
	cursor_pos_y = 3; cursor_pos_x = 0;
	atualizaIndiceInfo();paginaInfo();
}

// SUB-ROTINA PARA SELECAO DO MENU DE VISUALIZACAO
void menuSelect(void)
{	
	lerBtnEnter();
	if(flag_btn[enter])
	{
		resetBotoes();
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
				//paginaInfo();
				break;
			case 4:
				//paginaMe();
				break;
		}
	}
	resetBotoes();
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
	lerBtnEnter();
	if(flag_btn[enter])
	{
		resetBotoes();
		LED_BTN_ENTER = !LED_BTN_ENTER;	
		cmdLCD_i2c(_LCD_LIMPA);
		switch(indice)
		{
			case 1://motores
				maskRelogio();
				setupRelogio();
				break;
			case 2://sensores
				maskRelogio();
				setupRelogio();
				break;
			case 3://relogio
				maskRelogio();
				setupRelogio();
				break;
		}
	}
	resetBotoes();
}


//------------------------------controle-botoes-analogicos------------------------------//
// SUB-ROTINA PARA LEITURA DE UMA PORTA ANALOGICA DO PIC
signed short lerAnalogico(char CANAL)
{
	ADCON0bits.CHS = CANAL;		// Seleciona canal A/D
	Delay100TCYx(1);				// Aguarda tempo para troca de canal antes de iniciar convers?o
	ADCON0bits.GO_DONE = 1;    	// Inicia a convers?o
	while(ADCON0bits.GO_DONE);  // Aguarda t?rmino da convers?o
	
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

// enter,left,up,right,down
void lerBtnEnter(void)
{
	VAN0 = lerAnalogico(0);
	estado_btn = 0;
	
	if ((faixas[enter] - VARIACAO) <= VAN0 && (faixas[enter] + VARIACAO) >= VAN0)
	{
		estado_btn = 1;
		if(estado_btn && !estado_anterior_btn[enter]) flag_btn[enter]= 1;
	}
	estado_anterior_btn[enter] = estado_btn;
}
void lerBtnLeft(void)
{
	VAN0 = lerAnalogico(0);
	estado_btn = 0;
	
	if ((faixas[left] - VARIACAO) <= VAN0 && (faixas[left] + VARIACAO) >= VAN0)
	{
		estado_btn = 1;
		if(estado_btn && !estado_anterior_btn[left]) flag_btn[left]= 1;
	}
	estado_anterior_btn[left] = estado_btn;
}

void lerBtnUp(void)
{
	VAN0 = lerAnalogico(0);
	estado_btn = 0;
	
	if ((faixas[up] - VARIACAO) <= VAN0 && (faixas[up] + VARIACAO) >= VAN0)
	{
		estado_btn = 1;
		if(estado_btn && !estado_anterior_btn[up]) flag_btn[up]= 1;
	}
	estado_anterior_btn[up] = estado_btn;
}

void lerBtnRight(void)
{
	VAN0 = lerAnalogico(0);
	estado_btn = 0;
	
	if ((faixas[right] - VARIACAO) <= VAN0 && (faixas[right] + VARIACAO) >= VAN0)
	{
		estado_btn = 1;
		if(estado_btn && !estado_anterior_btn[right]) flag_btn[right]= 1;
	}
	estado_anterior_btn[right] = estado_btn;
}

void lerBtnDown(void)
{
	VAN0 = lerAnalogico(0);
	estado_btn = 0;
	
	if ((faixas[down] - VARIACAO) <= VAN0 && (faixas[down] + VARIACAO) >= VAN0)
	{
		estado_btn = 1;
		if(estado_btn && !estado_anterior_btn[down]) flag_btn[down]= 1;
	}
	estado_anterior_btn[down] = estado_btn;
}

void atualizaBotoes(void)
{
	VAN0 = lerAnalogico(0);
	lerBotoes();
}

void resetBotoes(void)
{
	unsigned char i;
	for(i = 0; i < QTD_BTNS; i++)
	{
		flag_btn[i]= 0;
	}
}
//---------------------------------manipulacao-de-dados---------------------------------//

// SUB-ROTINA PARA CONVERSAO DE UM INT PARA CHAR
void convertShortToChar(unsigned short num, char *str) 
{
	
	int length = lenShort(num); // Come?amos com um d?gito para o caractere nulo
	int i = 0;
		
	// Converte o n?mero em uma string de caracteres
	for (i = length - 1; i >= 0; i--)
	{
		str[i] = num % 10 + '0'; // Converte o d?gito para seu caractere correspondente
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
   if(PIR1bits.TMR1IF == 1)
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
      setTimer1();
   }
}

void configInterrupcaoRelogio(void)
{
   // Configura??o das interrup??es	
   
   RCONbits.IPEN 	= 0; // Desabilita atribui??o de prioridades ?s interrup??es
   INTCONbits.GIE 	= 1; // Habilita todas as interrup??es
   INTCONbits.PEIE 	= 1; // Habilita interrup??es geradas a partir de perif?ricos do PIC
   PIE1bits.TMR1IE	= 1; // Habilita interrup??o do Timer1
}

void configTimer1(void)
{
	T1CONbits.RD16		= 1; //contador de 8 bits (0) / contador de 16 bits (1)
	T1CONbits.T1RUN 	= 1; //fonte de clock: oscilador interno (1) / outra fonte (0)
	T1CONbits.T1CKPS1	= 0; // T1CKPS1:T1CKPS0 ? Configura??o do prescaler ? 00 (1:1), 01 (1:2), 10 (1:4), 11 (1:8)
	T1CONbits.T1CKPS0	= 0;
	T1CONbits.T1OSCEN 	= 1; // (modo de eco. de ener.) ? habilita (1) desliga o oscilador interno (0)
	T1CONbits.T1SYNC	= 0; // (somente no modo contador) ? modo s?ncrono (0) ou ass?ncrono (1)
	T1CONbits.TMR1CS	= 1; //modo contador ou economia de energia (1) / temporizador (0)
	T1CONbits.TMR1ON	= 1; //habilita o contador (1) , para o contador (0)
	setTimer1();
	
}

void setTimer1(void)
{
   PIR1bits.TMR1IF = 0;
   TMR1H = 0xC0;          
   TMR1L = 0x00;
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
	
	//PIE1bits.TMR1IE = !PIE1bits.TMR1IE; // Desabilita interrupcao do Timer1
	cursor_pos_y = 0; cursor_pos_x = 12;

	while(!flag_btn[left])
	{
		lerBtnUp();
		lerBtnDown();
		lerBtnLeft();

		setCursorLCD_i2c(0, cursor_pos_x+2); printShortLCD_i2c(hours);
		setCursorLCD_i2c(1, cursor_pos_x+2); printShortLCD_i2c(mins);
		setCursorLCD_i2c(2, cursor_pos_x+2); printShortLCD_i2c(secs);

		if (flag_btn[up])
		{
			flag_btn[up] = 0;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x); printCharLCD_i2c('  ');
			if (cursor_pos_y == 0) cursor_pos_y = 2;
			else cursor_pos_y --;
		}

		if (flag_btn[down])
		{
			flag_btn[down] = 0;
			setCursorLCD_i2c(cursor_pos_y, cursor_pos_x); printCharLCD_i2c('  ');
			if (cursor_pos_y == 2) cursor_pos_y = 0;
			else cursor_pos_y ++;
		}

		setCursorLCD_i2c(3, 19);printShortLCD_i2c(indice);
		setCursorLCD_i2c(cursor_pos_y, cursor_pos_x);piscaCursor();
	}
	resetBotoes();
	cursor_pos_y = 1, cursor_pos_x = 0;
	//cmdLCD_i2c(_LCD_LIMPA);
}

