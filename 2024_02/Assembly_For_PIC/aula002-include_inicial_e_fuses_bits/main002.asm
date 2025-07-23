;MCU:		PIC16F84A
;Clock:		4MHz
;Data:		26072024

list p=16F84A					; microcontrolador utilizado

; --- Arquivos incluidos no projeto ---
#include <p16f84a.inc> 			; inclui o arquivo do PIC16F84A


; --- FUSES Bits ---
;_XT_OSC -> 	tipo de oscilador
;_WDT_OFF -> 	desabilita o reset do PIC para eventos especificos
;_PWRTE_ON ->	habilita o aguardo de 72ms durante a inicializacao para garantir estavilidade dos registradores 
;_CP_OFF ->		desabilita protecao contra copias de codigo do PIC
	__config _XT_OSC & _WDT_OFF & _PWRTE_ON & _CP_OFF 