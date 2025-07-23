;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		28072024	Aula:012
;
;Instrucoes Aritmeticas
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

;--- INSTRUCOES ARITMETICAS 
; ---------------------------------
; ADDLW		k
; Operacao: W = W + k
; ---------------------------------
; ADDWF		f,d
; Operacao: d = W + f
; d = 0 (W) ou d = 1 (f)
; ---------------------------------
; RLF		f,d
; Operacao: d = f << 1 (rotaciona o registrador f um bit para a esquerda 'multiplica por 2')
; d = 0 (W) ou d = 1 (f)
; ---------------------------------
; RRF		f,d
; Operacao: d = f >> 1 (rotaciona o registrador f um bit para a direita 'divide por 2')
; d = 0 (W) ou d = 1 (f)
; ---------------------------------
; SUBLW		k
; Operacao: W = k - W
; ---------------------------------
; SUBWF		f,d
; Operacao: d = f - W
; d = 0 (W) ou d = 1 (f)
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
			; soma			
			movlw		D'10'			; W = 10
			addlw		D'35'			; W = W + 35 = 10 + 35 = 45		
			
			movlw		H'AC'			; W = H'AC'
			movwf		regist1			; regist1 = W
			addwf		regist1,W		; W = W + regist1
			
			; subtracao
			movlw		D'2'			; W = 2
			sublw		D'5'			; W = 5 - 2 = 5 - 2 = 3
			
			movlw		D'30'			; W = 30
			movwf		regist2			; regist2 = W
			movlw		D'1'			; W = 1
			subwf		regist2, regist2	; regist2 = regist2 - W
			
			; rotacao a esquerda (multiplicacao por 2)
			movlw		D'5'			; W = 5
			movwf		regist2			; regist2 = W
			rlf			regist2,W		; W = regist2 << 1
			
			; rotacao a direita (divide por 2)
			movlw		D'12'			; W = 12
			movwf		regist2			; regist2 = W
			rrf			regist2,W		; W = regist2 >> 1
			
			goto 		loop			; volta para a label loop



;--- Desenvolvimento das Sub-rotinas --------------------------------------------------------------------------						

;--------------------------------------------------------------------------------------------------------------				
			end							; final do programa
