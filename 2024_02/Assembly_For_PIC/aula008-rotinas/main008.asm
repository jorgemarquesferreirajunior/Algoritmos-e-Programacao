;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		28072024	Aula:008
;
;Aciona LED1 ligado em RB1, a partir do botao1 ligado em RB0
;Aciona LED2 ligado em RB3, a partir do botao2 ligado em RB2
;
;OBS:
;
;LEDs ligado em current sourcing:
;
; '0' - apaga
; '1' - acende
;
;BOTOES ligados em pull up
;
; '0' - acionado
; '1' - liberado
;

list p=16F84A							; microcontrolador utilizado


; --- Arquivos incluidos no projeto ---------------------------------------------------------------------------
#include <p16f84a.inc> 					; inclui o arquivo do PIC16F84A


; --- FUSES Bits ----------------------------------------------------------------------------------------------
;_XT_OSC -> 	tipo de oscilador
;_WDT_OFF -> 	desabilita o reset do PIC para eventos especificos
;_PWRTE_ON ->	habilita o aguardo de 72ms durante a inicializacao para garantir estavilidade dos registradores 
;_CP_OFF ->		desabilita protecao contra copias de codigo do PIC
	__config _XT_OSC & _WDT_OFF & _PWRTE_ON & _CP_OFF


; -- Pagina de Memoria ----------------------------------------------------------------------------------------
#define 	bank0		bcf STATUS,RP0	; Cria um mnemonico para o banco 0 de memoria
#define 	bank1		bsf	STATUS,RP0	; Cria um mnemonico para o banco 1 de memoria

; --- Termos do Set de instrucoes
;W: work (registrador de trabalho ou acumulador)
;L: literal (constante, numero. Utilizado como 'L' nas instrucoes e 'k' nos argumentos)
;D: destination (lical onde o resultado de uma operacao sera armazenado)
;B: bit (bits dentro dos registrados, dentro de um byte, flags)
;T: test (utilizado para testes de bits)
;S: skip (pulo/salto/desvio condicional)
;S: set (setar/tornar nivel alto)
;C: clear (limpar/tornar nivel baixo)
;Z: zero (testar se eh equivalente a zero)


;--- ENTRADAS -------------------------------------------------------------------------------------------------
#define 	botao1		PORTB,RB0		; botao1 ligado em RB0
#define 	botao2		PORTB,RB2		; botao2 ligado em RB2


;--- SAIDAS ---------------------------------------------------------------------------------------------------
#define 	led1		PORTB,RB1		; led1 ligado em RB1
#define		led2		PORTB,RB3		; led2 ligado em RB3


; --- Vetor de RESET ------------------------------------------------------------------------------------------
			org			H'0000'			; Origem no endereco 0000h de memoria
			goto		inicio			; Desvia do vetor de interrupcao 
			

; --- Vetor de Interrupcao ------------------------------------------------------------------------------------
			org			H'0004'			; Todas as interrupcoes apontam para esse endereco
			retfie						; Retorna da interrupcao


;--- Programa Principal ---------------------------------------------------------------------------------------
inicio:
			bank1						; seleciona o banco de memoria 1
			movlw		H'FF'			; W = B'01111111' movendo o valor H'FF' ao registrador de trabalho
			movwf		TRISA			; TRISA = H'FF' configurando o registrador de entradas e saidas A
			movlw		H'F5'			; W = B'1111 0101' movendo o valor H'AF' ao registrador de trabalho
			movwf		TRISB			; TRISB = H'AF' configurando o registrador de entradas e saidas B
			bank0						; seleciona o banco de memoria 0
			movlw		H'F5'			; W = B'1111 0101' movendo o valor H'F5' ao registrador de trabalho
			movwf		PORTB			; PORTB = H'F5'LEDs iniciam desligados
loop:									; loop infinito
			call 		monitora_botao1 ; chama sub-rotina monitora_botao1
			call 		monitora_botao2 ; chama sub-rotina monitora_botao2
			goto 		loop			; volta para a label loop

;--- Desenvolvimento das Sub-rotinas --------------------------------------------------------------------------
monitora_botao1:						; Sub-rotina botao1
			btfsc		botao1			; testa se o botao1 em pull up foi presionado
			goto		apaga_led1		; nao foi pressionado, desvia para a etapa apaga_led1
			bsf			led1			; botao pressionado, liga o led1
			return						; retorna da sub-rotina
			
apaga_led1:
			bcf			led1			; apaga led1
			return						; retorna da sub-rotina

monitora_botao2:						; Sub-rotina botao2
			btfsc		botao2			; testa se o botao2 em pull up foi presionado
			goto		apaga_led2		; nao foi pressionado, desvia para a etapa apaga_led2
			bsf			led2			; botao pressionado, liga o led2
			return						; retorna da sub-rotina
			
apaga_led2:
			bcf			led2			; apaga led2
			return						; retorna da sub-rotina
			
			
			end							; final do programa
