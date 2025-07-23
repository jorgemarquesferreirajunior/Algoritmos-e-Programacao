;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		26072024	Aula:004

list p=16F84A					; microcontrolador utilizado


; --- Arquivos incluidos no projeto ---------------------------------------------------------------------------
#include <p16f84a.inc> 			; inclui o arquivo do PIC16F84A


; --- FUSES Bits ----------------------------------------------------------------------------------------------
;_XT_OSC -> 	tipo de oscilador
;_WDT_OFF -> 	desabilita o reset do PIC para eventos especificos
;_PWRTE_ON ->	habilita o aguardo de 72ms durante a inicializacao para garantir estavilidade dos registradores 
;_CP_OFF ->		desabilita protecao contra copias de codigo do PIC
	__config _XT_OSC & _WDT_OFF & _PWRTE_ON & _CP_OFF


; -- Pagina de Memoria ----------------------------------------------------------------------------------------
#define 	bank0		bcf STATUS,RP0		; Cria um mnemonico para o banco 0 de memoria
#define 	bank1		bsf	STATUS,RP0		; Cria um mnemonico para o banco 1 de memoria

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


;--- SAIDAS ---
#define 	led1		PORTB,RB7		; led1 ligado em RB7


; --- Vetor de RESET ---
			org			H'0000'			; Origem no endereco 0000h de memoria
			goto		inicio			; Desvia do vetor de interrupcao 
			

; --- Vetor de Interrupcao ---
			org			H'0004'			; Todas as interrupcoes apontam para esse endereco
			retfie						; Retorna da interrupcao


;--- Programa Principal ---------------------------------------------------------------------------------------
inicio:
			bank1						; seleciona o banco de memoria 1
			movlw		B'01111111'		; W = B'01111111' movendo o valor H'7F' ao registrador de trabalho
			movwf		TRISB			; TRISB = H'7F' configurando o registrador de entradas e saidas B
			bank0						; seleciona o banco de memoria 0
			
			end							; final do programa
