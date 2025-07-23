;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		28072024	Aula:011
;
;Aciona LED1 ligado em RB1 e desliga LED2 ligado em RB3
;aguarda 500ms
;Aciona LED2 ligado em RB3 e desliga LED1 ligado em RB1
;Aguarda 500ms
;
;OBS:
;
;LEDs ligado em current sourcing:
;
; '0' - apaga
; '1' - acende
;
;Calculo de ciclo de maquina
;
;
;
;
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
 
;--- ENTRADAS -------------------------------------------------------------------------------------------------
#define 	botao1		PORTB,RB0		; botao ligado em RB0
#define 	botao2		PORTB,RB2		; botao ligado em RB2

;--- SAIDAS ---------------------------------------------------------------------------------------------------
#define 	led1		PORTB,RB1		; led1 ligado em RB1
#define		led2		PORTB,RB3		; led2 ligado em RB3

; --- Registradores de Uso Geral ------------------------------------------------------------------------------
cblock					H'0C'			; configura o endereco de memoria inicial para uso geral
			tempo1						; linka a variavel tempo1 ao endereco H'0C'
			tempo2						; linka a variavel tempo1 ao endereco H'0D'
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
			movlw		H'F5'			; W = B'1111 0101' -> entradas <RB7:RB4,RB2,RB0 > saidas <RB1,RB3> 
			movwf		TRISB			; TRISB = H'F5' configurando o registrador de entradas e saidas B
			bank0						; seleciona o banco de memoria 0
			movlw		H'F5'			; W = B'1111 0101' movendo o valor H'F5' ao registrador de trabalho
			movwf		PORTB			; PORTB = H'F5'LEDs iniciam desligados
loop:									; loop infinito
			bsf			led1			; liga o led1
			bcf			led2			; desliga o led2
			call 		delay500ms		; rotina de delay
			bcf			led1			; desliga led1
			bsf			led2			; liga led2
			call 		delay500ms		; rotina de delay
			
			goto 		loop			; volta para a label loop

;--- Desenvolvimento das Sub-rotinas --------------------------------------------------------------------------
delay500ms:
			movlw		D'200'			; W = 200
			movwf		tempo1			; salva o valor 200 na variavel tempo1
										; 2 ciclos gastos
			
aux1:
			movlw		D'250'			; w = 250
			movwf		tempo2			; salva o valor 250 na variavel tempo2
										; 2 ciclos gastos
aux2:
			nop							; apenas gasta 1 ciclo de processamento do microcontrolador
			nop
			nop
			nop
			nop
			nop
			nop
			
			decfsz		tempo2			; decrementa uma unidade do valor salvo na varaivel tempo2
			goto 		aux2			; retorna ao inicio da sub-rotina aux2
										; OBS: quando o valor for zero, pula o goto. Isso ocorre por conta
										; do comando anterior decfsz (drecementa um dado e pula o proximo 
										; comando quando o valor for zero)
										
										; 10 ciclos gastos em uma sub-rotina em aux2
										; eh repetida a sub-rotina o total de vezes do valor salvo em H'0D'
										; 250 x 10 ciclos =  2500 ciclos
										
			decfsz		tempo1			; decrementa uma unidade do valor salvo na variavel  tempo1
			goto 		aux1			; retorna ao inicio da sub-rotina aux1
										; 3 ciclos gastos em uma repeticao da sub-rotina aux1
										; eh repetida a sub-rotina o total de vezes do valor salvo em H'0C'
										; 200 x 2500 ciclos = 500000 ciclos 
										; delay = tempo de ciclos X quanntidade de ciclos em vazio
										; delay = 1us x 500000 = 500ms
			return						; retorna apos a chamada da sub rotina
										
			
			
			
			end							; final do programa
