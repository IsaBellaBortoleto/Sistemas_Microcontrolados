; display.s

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------

; Declarações EQU - Defines
; ========================
; Definições dos Registradores Gerais
SYSCTL_RCGCGPIO_R	 EQU	0x400FE608
SYSCTL_PRGPIO_R		 EQU    0x400FEA08
    

; Definições dos Ports
; Serão utilizados os ports A, B, P e Q

; PORT A
GPIO_PORTA_AHB_LOCK_R     EQU    0x40058520
GPIO_PORTA_AHB_CR_R       EQU    0x40058524
GPIO_PORTA_AHB_AMSEL_R    EQU    0x40058528
GPIO_PORTA_AHB_PCTL_R     EQU    0x4005852C
GPIO_PORTA_AHB_DIR_R      EQU    0x40058400
GPIO_PORTA_AHB_AFSEL_R    EQU    0x40058420
GPIO_PORTA_AHB_DEN_R      EQU    0x4005851C
GPIO_PORTA_AHB_DATA_R     EQU    0x400583FC
GPIO_PORTA                EQU    2_000000000000001
	
; PORT B
GPIO_PORTB_AHB_LOCK_R     EQU    0x40059520
GPIO_PORTB_AHB_CR_R       EQU    0x40059524
GPIO_PORTB_AHB_AMSEL_R    EQU    0x40059528
GPIO_PORTB_AHB_PCTL_R     EQU    0x4005952C
GPIO_PORTB_AHB_DIR_R      EQU    0x40059400
GPIO_PORTB_AHB_AFSEL_R    EQU    0x40059420
GPIO_PORTB_AHB_DEN_R      EQU    0x4005951C
GPIO_PORTB_AHB_DATA_R     EQU    0x400593FC
GPIO_PORTB                EQU    2_000000000000010

; PORT P
GPIO_PORTP_LOCK_R     EQU    0x40065520
GPIO_PORTP_CR_R       EQU    0x40065524
GPIO_PORTP_AMSEL_R    EQU    0x40065528
GPIO_PORTP_PCTL_R     EQU    0x4006552C
GPIO_PORTP_DIR_R      EQU    0x40065400
GPIO_PORTP_AFSEL_R    EQU    0x40065420
GPIO_PORTP_DEN_R      EQU    0x4006551C
GPIO_PORTP_DATA_R     EQU    0x400653FC
GPIO_PORTP            EQU    2_010000000000000

; PORT Q
GPIO_PORTQ_LOCK_R     EQU    0x40066520
GPIO_PORTQ_CR_R       EQU    0x40066524
GPIO_PORTQ_AMSEL_R    EQU    0x40066528
GPIO_PORTQ_PCTL_R     EQU    0x4006652C
GPIO_PORTQ_DIR_R      EQU    0x40066400
GPIO_PORTQ_AFSEL_R    EQU    0x40066420
GPIO_PORTQ_DEN_R      EQU    0x4006651C
GPIO_PORTQ_DATA_R     EQU    0x400663FC
GPIO_PORTQ            EQU    2_100000000000000



; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
		
        EXPORT Display_Init
		EXPORT PortA_Output
		EXPORT PortQ_Output
		EXPORT PortB_Output
		EXPORT PortP_Output
		EXPORT Mostra_Tudo
			
		IMPORT SysTick_Wait1ms
									

;--------------------------------------------------------------------------------
; Função Display_Init
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
Display_Init
;=====================
;=====================
; 1. Ativar o clock para as portas setando o bit correspondente no registrador RCGCGPIO,
; após isso verificar no PRGPIO se as portas estão prontas para uso.

            LDR     R0, =SYSCTL_RCGCGPIO_R
			LDR     R1, [R0] ;preservamos os bits já habilitados em GPIO_Init
			ORR     R1, #GPIO_PORTA
			ORR     R1, #GPIO_PORTB
			ORR     R1, #GPIO_PORTP
			ORR     R1, #GPIO_PORTQ
			STR     R1, [R0]
; Esperar as portas ficarem prontas
            LDR     R0, =SYSCTL_PRGPIO_R
			
EsperaGPIO  LDR     R1, [R0]
            MOV     R2, #GPIO_PORTA
            ORR     R2, #GPIO_PORTB
            ORR     R2, #GPIO_PORTP
            ORR     R2, #GPIO_PORTQ
            TST     R1, R2
            BEQ     EsperaGPIO

; 2. Limpar o AMSEL para desabilitar a analógica
            MOV     R1, #0x00
            LDR     R0, =GPIO_PORTA_AHB_AMSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTB_AHB_AMSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTP_AMSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTQ_AMSEL_R
            STR     R1, [R0]
			
; 3. Limpar PCTL para selecionar o GPIO
            MOV     R1, #0x00
            LDR     R0, =GPIO_PORTA_AHB_PCTL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTB_AHB_PCTL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTP_PCTL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTQ_PCTL_R
            STR     R1, [R0]

; 4. DIR para 0 se for entrada, 1 se for saída
;Nesse caso, A, B, P e Q serão configurados como saída
; PA7:PA4
            LDR     R0, =GPIO_PORTA_AHB_DIR_R
            MOV     R1, #2_11110000
            STR     R1, [R0]
; PB5:PB4
			LDR 	R0, =GPIO_PORTB_AHB_DIR_R
			MOV     R1, #2_00110000
			STR     R1, [R0]
; PP5
            LDR     R0, =GPIO_PORTP_DIR_R
            MOV     R1, #2_00100000
            STR     R1, [R0]
; PQ3:PQ0
            LDR     R0, =GPIO_PORTQ_DIR_R
            MOV     R1, #2_00001111
            STR     R1, [R0]
			
; 5. Limpar os bits AFSEL para 0 para selecionar GPIO 
;    Sem função alternativa
            MOV     R1, #0x00
            LDR     R0, =GPIO_PORTA_AHB_AFSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTB_AHB_AFSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTP_AFSEL_R
            STR     R1, [R0]
            LDR     R0, =GPIO_PORTQ_AFSEL_R
            STR     R1, [R0]

; 6. Setar os bits de DEN para habilitar I/O digital
; PA7:PA4
            LDR     R0, =GPIO_PORTA_AHB_DEN_R
            MOV     R1, #2_11110000
            STR     R1, [R0]

; PB5:PB4
            LDR     R0, =GPIO_PORTB_AHB_DEN_R
            MOV     R1, #2_00110000
            STR     R1, [R0]

; PP5
            LDR     R0, =GPIO_PORTP_DEN_R
            MOV     R1, #2_00100000
            STR     R1, [R0]

; PQ3:PQ0
            LDR     R0, =GPIO_PORTQ_DEN_R
            MOV     R1, #2_00001111
            STR     R1, [R0]

            BX      LR
			
PortA_Output
; Entrada: R1 = código de 8 bits do display
; R2 = temporário
        LDR     R0, =GPIO_PORTA_AHB_DATA_R
        MOV     R2, R1
        AND     R2, #2_11110000
        STR     R2, [R0]
        BX      LR

PortQ_Output
; Entrada: R1 = código de 8 bits do display
; R2 = temporário
        LDR     R0, =GPIO_PORTQ_DATA_R
        MOV     R2, R1
        AND     R2, #2_00001111
        STR     R2, [R0]
        BX      LR

PortB_Output
; Entrada: R1 = valor de controle para PB5:PB4
; R2 = temporário
        LDR     R0, =GPIO_PORTB_AHB_DATA_R
        MOV     R2, R1
        AND     R2, #2_00110000
        STR     R2, [R0]

        BX      LR
		
PortP_Output
; Entrada: R1 = valor de controle para PP5
; R2 = temporário
        LDR     R0, =GPIO_PORTP_DATA_R
        MOV     R2, R1
        AND     R2, #2_00100000
        STR     R2, [R0]

        BX      LR

Mostra_Tudo
;aqui vai a multiplexação. A ideia seria ativar com a temporização cada display e antes de cada display de 7 seg chamar Numero_Display
; Entrada:
; R0 = código da dezena
; R1 = código da unidade
; R2 = setpoint
		MOV     R4, R0      ; guarda dezena
        MOV     R5, R1      ; guarda unidade
        MOV     R6, R2      ; guarda setpoint
		
		PUSH {LR}; para poder voltar a main
		
;dezena:
		MOV R1, R4
		BL PortA_Output
		
		MOV R1, R4
		BL PortQ_Output

		MOV R1, #2_00010000 ;ativamos o transistor Q2 pelo pino PB4
		BL      PortB_Output
		
		MOV     R0, #1
        BL      SysTick_Wait1ms
		
		MOV     R1, #0x00             ; desativa Q2
        BL      PortB_Output
		
		MOV     R0, #1
        BL      SysTick_Wait1ms
		
;unidade:
		MOV     R1, R5
        BL      PortA_Output

        MOV     R1, R5
        BL      PortQ_Output

        MOV R1, #2_00100000      ;ativamos o transistor Q3 pelo pino PB5
        BL      PortB_Output

        MOV     R0, #1
        BL      SysTick_Wait1ms

        MOV     R1, #0x00             ; desativa Q3
        BL      PortB_Output

        MOV     R0, #1
        BL      SysTick_Wait1ms

;LEDs:
		MOV     R1, R6
        BL      PortA_Output      ; pega bits 7:4

        MOV     R1, R6
        BL      PortQ_Output      ; pega bits 3:0

        MOV     R1, #2_00100000    ;ativamos o transistor Q1 pelo pino PP5
        BL      PortP_Output

        MOV     R0, #1
        BL      SysTick_Wait1ms

        MOV     R1, #0x00          ; desativa Q1
        BL      PortP_Output

        MOV     R0, #1
        BL      SysTick_Wait1ms
		
		POP {LR} 
		BX LR 