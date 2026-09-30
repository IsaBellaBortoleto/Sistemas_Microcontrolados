; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 15/03/2018
; Este programa espera o usuário apertar a chave USR_SW1 e/ou a chave USR_SW2.
; Caso o usuário pressione a chave USR_SW1, acenderá o LED2. Caso o usuário pressione 
; a chave USR_SW2, acenderá o LED1. Caso as duas chaves sejam pressionadas, os dois 
; LEDs acendem.

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
			
N_1S EQU 167 ;167 voltas x 6ms = 1s
; -------------------------------------------------------------------------------

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		EXPORT TEMP_ALVO [DATA,SIZE=4]
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		
TEMP_ATUAL SPACE 4
TEMP_ALVO SPACE 4
CONTADOR SPACE 4
	
; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms										
		IMPORT  GPIO_Init
        IMPORT  PortN_Output
			
		IMPORT Display_Init
		IMPORT Mostra_Tudo
; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init              ;Chama a subrotina para inicializar o SysTick
	
	;TEMP_ATUAL=10
	LDR R0, =TEMP_ATUAL
	MOV R1, #10
	STR R1, [R0]
	
	;TEMP_ALVO=22
	LDR R0, =TEMP_ALVO
	MOV R1,#22
	STR R1,[R0]
	
	;CONTADOR=167
	LDR R0, =CONTADOR
	MOV R1,#N_1S
	STR R1,[R0]
	
	BL GPIO_Init                 ;Chama a subrotina que inicializa os GPIO
	BL Display_Init				;Chama a subrotina que inicializa os GPIO ligados à PAT


	
MainLoop

	LDR R4,=TEMP_ATUAL
	LDR R4,[R4]
	LDR R5,=TEMP_ALVO
	LDR R5,[R5]
	CMP R4,R5
	
	BLO temp_menor ;atual < alvo = aquecimento
	BHI temp_maior ;atual > alvo = resfriamento
	MOV R0,#2_01
	MOV R1,#2_10
	ORR R0,R1 ;Acende os dois LEDs 
	B atualizaLED
	
temp_menor ;Acende o LED de aquecimento(PN0)
	MOV R0,#2_01
	B atualizaLED
	
temp_maior ;Acende o LED de resfriamento(PN1)
	MOV R0,#2_10 

atualizaLED
	BL PortN_Output
	
	;Dezena com UDIV
	MOV R0,#10
	UDIV R6,R4,R0
	
	;Unidade com MLS
	MLS R7,R6,R0,R4
	LDR  R0, =TABELA_7SEG
	
    LDRB R8, [R0, R6]         
    LDRB R9, [R0, R7]          

;passamos aqui a entrada para a função Mostra_Tudo 
    MOV  R0, R8              
    MOV  R1, R9
    MOV  R2, R5
    BL   Mostra_Tudo        
	
	
;	;O atraso provisório de 6 ms - REMOVER DEPOIS
;	MOV R0,#6
;	BL SysTick_Wait1ms
	
	;Contar as voltas
	LDR R0, =CONTADOR
	LDR R1,[R0]
	SUBS R1,R1,#1
	STR R1,[R0]
	BNE fim_seg 
	
	;Chegou a zero, recarregar o contador
	MOV R1,#N_1S
	STR R1,[R0]
	
	;Aproximar a temperatura do alvo
	LDR R2,=TEMP_ATUAL
	LDR R4,[R2]
	LDR R3,=TEMP_ALVO
	LDR R5,[R3]
	CMP R4,R5
	BLO sobe ;atual < alvo = aquecimento
	BHI desce;atual > alvo = resfriamento

	B fim_seg
	
sobe
	ADD R4,R4,#1
	B salva_temp
desce
	SUB R4,R4,#1
salva_temp
	STR R4,[R2]
	
fim_seg	
	B MainLoop                   ;Volta para o laço principal	
	
TABELA_7SEG DCB 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo