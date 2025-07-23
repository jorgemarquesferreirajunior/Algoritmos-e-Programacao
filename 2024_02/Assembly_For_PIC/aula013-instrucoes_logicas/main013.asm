;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		28072024	Aula:013
;
;Instrucoes Logicas
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

; OBS: TEMPOS DE CICLO
; movwf		1 ciclo
; movlw		1 ciclo
; goto
; call
; return

;--- INSTRUCOES LOGICAS 
; ---------------------------------
; ANDLW		k
; Operacao: W = W AND k
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; k = 1101 0001
; andlw k
; W = 0001 0000
; ---------------------------------
; ADDWF		f,d
; Operacao: d = W AND f
; d = 0 (W) ou d = 1 (f)
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; f = 1101 0001
; andwf f,aux1
; aux1 = 0001 0000
; ---------------------------------
; COMF		f,d
; Operacao: d = NOT f
; d = 0 (W) ou d = 1 (f)
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; f = 1101 0001
; comf f,f
; f = 0010 1110
; ---------------------------------
; IORLW		k
; Operacao: W = W OR k
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; k = 1101 0001
; iorlw k
; W = 1101 0111
; ---------------------------------
; IORWF		f,d
; Operacao: d = W OR f
; d = 0 (W) ou d = 1 (f)
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; f = 1101 0001
; andwf f,aux1
; aux1 = 1101 0111
; ---------------------------------
; XORLW		k
; Operacao: W = W XOR k
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; k = 1101 0001
; iorlw k
; W = 1100 0111
; ---------------------------------
; XORWF		f,d
; Operacao: d = W XOR f
; d = 0 (W) ou d = 1 (f)
; OBS: afeta a flag Z do registrador STATUS
; exemplo
; W = 0001 0110
; f = 1101 0001
; andwf f,aux1
; aux1 = 1100 0111
; ---------------------------------
 
;--- ENTRADAS -------------------------------------------------------------------------------------------------
;--- SAIDAS ---------------------------------------------------------------------------------------------------
; --- Registradores de Uso Geral ------------------------------------------------------------------------------
cblock		H'0C'						; configura o endereco de memoria inicial para uso geral

			regist1
			regist2
endc									; finaliza o gerenciamento de memoria para geristradores de uso geral


; Opcao 2
; tempo1 equ	H'0C'
; tempo2 equ	H'0D'

; --- Vetor de RESET ------------------------------------------------------------------------------------------
			org			H'0000'			; Origem no endereco 0000h de memoria
			goto		inicio			; Desvia do vetor de interrupcao 
			

; --- Vetor de Interrupcao ------------------------------------------------------------------------------------
			org			H'0004'			; Todas as interrupcoes apontam para esse endereco
			retfie						; Retorna da interrupcao


;--- Programa Principal ---------------------------------------------------------------------------------------
inicio:
			bank1						; seleciona o banco de memoria 1
			movlw		H'FF'			; W = H'FF'
			movwf		TRISA			; TRISB = B'1111 1111' -> entradas <RB7:0 > saidas <>
			movlw		H'FF'			; W = H'FF'
			movwf		TRISB			; TRISB = B'1111 1111' -> entradas <RB7:0 > saidas <>
			bank0						; seleciona o banco de memoria 0
			clrf		regist1			; zera o geristrador inteiro correspondente
			clrf		regist2			; zera o geristrador inteiro correspondente
			
loop:									; loop infinito
			; and			
			movlw		B'11110000'		; W = H'F0'
			andlw		B'10100001'		; W = W AND B'10100001'
			
			xorwf		regist1,F		; register1 = register1 XOR W
			
			goto 		loop			; volta para a label loop



;--- Desenvolvimento das Sub-rotinas --------------------------------------------------------------------------						

;--------------------------------------------------------------------------------------------------------------				
			end							; final do programa
