; gpio.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron - adaptado para o Lab 1 (GPIO e Interrupções)
; 19/03/2018
; Configuração do Port J (chaves com interrupção) e Port N (LEDs PN0/PN1)
; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
; ========================
; Definições dos Registradores Gerais
SYSCTL_RCGCGPIO_R	 EQU	0x400FE608
SYSCTL_PRGPIO_R		 EQU    0x400FEA08
    
; ========================
; NVIC
NVIC_EN2_R           EQU    0xE000E108
NVIC_PRI18_R		 EQU    0xE000E448

NVIC_EN1_R			 EQU	0xE000E104
NVIC_PRI12_R		 EQU	0xE000E430
; ========================
; Definições dos Ports
; PORT K
GPIO_PORTK_IS_R      	EQU    0x40061404
GPIO_PORTK_IBE_R      	EQU    0x40061408
GPIO_PORTK_IEV_R      	EQU    0x4006140C
GPIO_PORTK_IM_R      	EQU    0x40061410
GPIO_PORTK_RIS_R      	EQU    0x40061414
GPIO_PORTK_ICR_R      	EQU    0x4006141C    
GPIO_PORTK_LOCK_R    	EQU    0x40061520
GPIO_PORTK_CR_R      	EQU    0x40061524
GPIO_PORTK_AMSEL_R   	EQU    0x40061528
GPIO_PORTK_PCTL_R    	EQU    0x4006152C
GPIO_PORTK_DIR_R     	EQU    0x40061400
GPIO_PORTK_AFSEL_R   	EQU    0x40061420
GPIO_PORTK_DEN_R     	EQU    0x4006151C
GPIO_PORTK_PUR_R     	EQU    0x40061510	
GPIO_PORTK_DATA_R    	EQU    0x400613FC
GPIO_PORTK              EQU    2_00001000000000

; PORT M
GPIO_PORTM_IS_R      	EQU    0x40063404
GPIO_PORTM_IBE_R      	EQU    0x40063408
GPIO_PORTM_IEV_R      	EQU    0x4006340C
GPIO_PORTM_IM_R      	EQU    0x40063410
GPIO_PORTM_RIS_R      	EQU    0x40063414
GPIO_PORTM_ICR_R      	EQU    0x4006341C    
GPIO_PORTM_LOCK_R    	EQU    0x40063520
GPIO_PORTM_CR_R      	EQU    0x40063524
GPIO_PORTM_AMSEL_R   	EQU    0x40063528
GPIO_PORTM_PCTL_R    	EQU    0x4006352C
GPIO_PORTM_DIR_R     	EQU    0x40063400
GPIO_PORTM_AFSEL_R   	EQU    0x40063420
GPIO_PORTM_DEN_R     	EQU    0x4006351C
GPIO_PORTM_PUR_R     	EQU    0x40063510	
GPIO_PORTM_DATA_R    	EQU    0x400633FC
GPIO_PORTM              EQU    2_00100000000000
; ========================
; Definições dos Ports
; PORT J
GPIO_PORTJ_AHB_LOCK_R    	EQU    0x40060520
GPIO_PORTJ_AHB_CR_R      	EQU    0x40060524
GPIO_PORTJ_AHB_AMSEL_R   	EQU    0x40060528
GPIO_PORTJ_AHB_PCTL_R    	EQU    0x4006052C
GPIO_PORTJ_AHB_DIR_R     	EQU    0x40060400
GPIO_PORTJ_AHB_AFSEL_R   	EQU    0x40060420
GPIO_PORTJ_AHB_DEN_R     	EQU    0x4006051C
GPIO_PORTJ_AHB_PUR_R     	EQU    0x40060510	
GPIO_PORTJ_AHB_DATA_R    	EQU    0x400603FC
GPIO_PORTJ               	EQU    2_000000100000000
	
GPIO_PORTJ_AHB_IM_R			EQU	   0x40060410
GPIO_PORTJ_AHB_IS_R			EQU	   0x40060404
GPIO_PORTJ_AHB_IBE_R		EQU	   0x40060408
GPIO_PORTJ_AHB_IEV_R		EQU    0x4006040C
GPIO_PORTJ_AHB_ICR_R		EQU    0x4006041C
GPIO_PORTJ_AHB_MIS_R		EQU    0x40060418

	
; PORT N
GPIO_PORTN_AHB_LOCK_R    	EQU    0x40064520
GPIO_PORTN_AHB_CR_R      	EQU    0x40064524
GPIO_PORTN_AHB_AMSEL_R   	EQU    0x40064528
GPIO_PORTN_AHB_PCTL_R    	EQU    0x4006452C
GPIO_PORTN_AHB_DIR_R     	EQU    0x40064400
GPIO_PORTN_AHB_AFSEL_R   	EQU    0x40064420
GPIO_PORTN_AHB_DEN_R     	EQU    0x4006451C
GPIO_PORTN_AHB_PUR_R     	EQU    0x40064510	
GPIO_PORTN_AHB_DATA_R    	EQU    0x400643FC
GPIO_PORTN               	EQU    2_001000000000000	
GPIO_PORTN_DATA_R			EQU    0x400643FC

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT GPIO_Init            ; Permite chamar GPIO_Init de outro arquivo
		EXPORT PortN_Output			; Permite chamar PortN_Output de outro arquivo
        EXPORT GPIOPortJ_Handler    
        IMPORT EnableInterrupts
        IMPORT DisableInterrupts
		IMPORT SysTick_Wait1ms
		IMPORT TEMP_ALVO
									

;--------------------------------------------------------------------------------
; Função GPIO_Init
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
GPIO_Init
;=====================
; 1. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO,
; após isso verificar no PRGPIO se a porta está pronta para uso.
; enable clock to GPIOJ e GPION at clock gating register
            LDR     R0, =SYSCTL_RCGCGPIO_R  		;Carrega o endereço do registrador RCGCGPIO
			LDR     R1, [R0]                        ;Lê as portas já habilitadas
			ORR		R1, #GPIO_PORTJ                 ;Seta o bit da porta J
			ORR     R1, #GPIO_PORTN					;Seta o bit da porta N
            STR     R1, [R0]						;Move para a memória os bits das portas no endereço do RCGCGPIO
 
            LDR     R0, =SYSCTL_PRGPIO_R			;Carrega o endereço do PRGPIO para esperar os GPIO ficarem prontos
EsperaGPIO  LDR     R1, [R0]						;Lê da memória o conteúdo do endereço do registrador
			MOV     R2, #GPIO_PORTN                 ;Seta os bits correspondentes às portas para fazer a comparação
			ORR     R2, #GPIO_PORTJ                 ;Seta o bit da porta J, fazendo com OR
            TST     R1, R2							;ANDS de R1 com R2
            BEQ     EsperaGPIO					    ;Se o flag Z=1, volta para o laço. Senão continua executando
 
; 2. Limpar o AMSEL para desabilitar a analógica
            MOV     R1, #0x00						;Colocar 0 no registrador para desabilitar a função analógica
            LDR     R0, =GPIO_PORTJ_AHB_AMSEL_R     	;Carrega o R0 com o endereço do AMSEL para a porta J
            STR     R1, [R0]						;Guarda no registrador AMSEL da porta J da memória
			LDR     R0, =GPIO_PORTN_AHB_AMSEL_R     	;Carrega o R0 com o endereço do AMSEL para a porta N
            STR     R1, [R0]						;Guarda no registrador AMSEL da porta N da memória
     
 
; 3. Limpar PCTL para selecionar o GPIO
            MOV     R1, #0x00					    ;Colocar 0 no registrador para selecionar o modo GPIO
            LDR     R0, =GPIO_PORTJ_AHB_PCTL_R		;Carrega o R0 com o endereço do PCTL para a porta J
            STR     R1, [R0]                        ;Guarda no registrador PCTL da porta J da memória
			LDR     R0, =GPIO_PORTN_AHB_PCTL_R		;Carrega o R0 com o endereço do PCTL para a porta N
            STR     R1, [R0]                        ;Guarda no registrador PCTL da porta N da memória

; 4. DIR para 0 se for entrada, 1 se for saída
												
			LDR     R0, =GPIO_PORTJ_AHB_DIR_R		;Carrega o R0 com o endereço do DIR para a porta J
			MOV     R1, #2_00000000					;PJ1 & PJ0 para entrada
            STR     R1, [R0]						;Guarda no registrador
			; O certo era verificar os outros bits da PN para não transformar entradas em saídas desnecessárias
            LDR     R0, =GPIO_PORTN_AHB_DIR_R	   	;Carrega o R0 com o endereço do DIR para a porta N
            MOV     R1, #2_00000010					;Colocar 1 no registrador DIR para funcionar como saída
			MOV		R2, #2_00000001
			ORR 	R1, R2
            STR     R1, [R0]						
; 5. Limpar os bits AFSEL para 0 para selecionar GPIO 
;    Sem função alternativa
            MOV     R1, #0x00						;Colocar o valor 0 para não setar função alternativa
            LDR     R0, =GPIO_PORTJ_AHB_AFSEL_R     ;Carrega o endereço do AFSEL da porta J
            STR     R1, [R0]                        ;Escreve na porta			
            LDR     R0, =GPIO_PORTN_AHB_AFSEL_R		;Carrega o endereço do AFSEL da porta N
            STR     R1, [R0]						;Escreve na porta
            
; 6. Setar os bits de DEN para habilitar I/O digital
            LDR     R0, =GPIO_PORTN_AHB_DEN_R			;Carrega o endereço do DEN                                    
			MOV     R1, #2_00000010
			MOV		R2, #2_00000001
			ORR 	R1, R2
            STR     R1, [R0]                            ;Escreve no registrador da memória funcionalidade digital
			
            LDR     R0, =GPIO_PORTJ_AHB_DEN_R			;Carrega o endereço do DEN
            LDR     R1, [R0]							;Ler da memória o registrador GPIO_PORTJ_AHB_DEN_R
			MOV     R2, #2_00000001	
			ORR     R2, #2_00000010						;Habilitar funcionalidade digital na DEN os bits 0 e 1
            ORR     R1, R2
            STR     R1, [R0]							;Escreve no registrador da memória funcionalidade digital
			
; 7. Para habilitar resistor de pull-up interno, setar PUR para 1
			LDR     R0, =GPIO_PORTJ_AHB_PUR_R			;Carrega o endereço do PUR para a porta J
			LDR     R1, [R0]							;Ler da memória o registrador GPIO_PORTJ_AHB_PUR_R
			MOV     R2, #2_00000001	
			ORR     R2, #2_00000010						;Habilitar pull-up no PUR os bits 0 e 1
            ORR     R1, R2
            STR     R1, [R0]							;Escreve no registrador da memória do resistor de pull-up

;Interrupções
; 8. Desabilitar a interrupção no registrador IM
			LDR     R0, =GPIO_PORTJ_AHB_IM_R			;Carrega o endereço do IM para a porta J
			MOV     R1, #2_00							;Desabilitar as interrupções  
            STR     R1, [R0]							;Escreve no registrador
            
; 9. Configurar o tipo de interrupção por borda no registrador IS
			LDR     R0, =GPIO_PORTJ_AHB_IS_R			;Carrega o endereço do IS para a porta J
			MOV     R1, #2_00							;Por Borda  
            STR     R1, [R0]							;Escreve no registrador

; 10. Configurar  borda única no registrador IBE
			LDR     R0, =GPIO_PORTJ_AHB_IBE_R				;Carrega o endereço do IBE para a porta J
			MOV     R1, #2_00							;Borda Única  
            STR     R1, [R0]							;Escreve no registrador
; 11. Configurar  borda de descida (botão pressionado) no registrador IEV
			LDR     R0, =GPIO_PORTJ_AHB_IEV_R			;Carrega o endereço do IEV para a porta J
			MOV     R1, #2_00  							; 0 = descida, para os dois pinos
            STR     R1, [R0]							;Escreve no registrador
  
 ;ICR - limpa flags pendentes antes de habilitar a interrupção
			LDR     R0, =GPIO_PORTJ_AHB_ICR_R
			MOV     R1, #2_01
			MOV  	R2, #2_10
			ORR		R1, R2
            STR     R1, [R0]
  
; 12. Habilitar a interrupção no registrador IM
			LDR     R0, =GPIO_PORTJ_AHB_IM_R				;Carrega o endereço do IM para a porta J
			MOV     R1, #2_01
			MOV  	R2, #2_10
			ORR		R1, R2
            STR     R1, [R0]							;Escreve no registrador
            
;Interrupção número 51            
; 13. Setar a prioridade no NVIC
			LDR     R0, =NVIC_PRI12_R           		;Carrega o do NVIC para o grupo que tem o J entre 48 e 51
			MOV     R1, #5 		                   		;Prioridade 5
			LSL     R1, R1, #29							;Desloca 29 bits para a esquerda já que o J é o último byte do PRI12
            STR     R1, [R0]							;Escreve no registrador da memória
; 14. Habilitar a interrupção no NVIC
			LDR     R0, =NVIC_EN1_R           			;Carrega o do NVIC para o grupo que tem o J entre 32 e 63
			MOV     R1, #1
			LSL     R1, #19								;Desloca 19 bits para a esquerda já que o J é a interrupção do bit 19 no EN1
            STR     R1, [R0]							;Escreve no registrador da memória


			BX  LR

; -------------------------------------------------------------------------------
; Função PortN_Output
; Parâmetro de entrada: R0 --> se os BIT1 e BIT0 estão ligado ou desligado
; Parâmetro de saída: Não tem
PortN_Output
	LDR	R1, =GPIO_PORTN_DATA_R		    ;Carrega o valor do offset do data register
	;Read-Modify-Write para escrita
	LDR R2, [R1]
	BIC R2, #2_00000011                     ;Primeiro limpamos os dois bits do lido da porta R2 = R2 & 11111100
	ORR R0, R0, R2                          ;Fazer o OR do lido pela porta com o parâmetro de entrada
	STR R0, [R1]                            ;Escreve na porta N o barramento de dados dos pinos [N5-N0]
	BX LR									;Retorno





; -------------------------------------------------------------------------------
; Função ISR GPIOPortJ_Handler (Tratamento da interrupção)
GPIOPortJ_Handler

	LDR R2, =GPIO_PORTJ_AHB_MIS_R 	;carrega o endereço do MIS
	LDR R2,[R2] 					;R2=MIS
	
	
	LDR R0, =GPIO_PORTJ_AHB_ICR_R 	;carrega o endereço do ICR
    STR R2, [R0] 					;ACK, limpa os flags que dispararam
	
	LDR R0,=TEMP_ALVO
	LDR R1,[R0] 					;R1=valor atual da temp alvo
	

;SW1 - incrementa o alvo	
	TST R2,#2_01 					;testa o bit 0 do MIS
	BEQ testaSW2 					;se bit 0=0, SW1 não foi presssionada, pula
	
	
	CMP R1,#50 						;compara o alvo com 50
	BHS testaSW2 					;alvo>=50, pula
	ADDS R1,R1,#1
	
;SW2 - decrementa o alvo	

testaSW2
	TST R2,#2_10 					;testa o bit 1 do MIS
	BEQ salvaAlvo 					;se bit 1=0, SW2 não foi presssionada, pula
	
	
	CMP R1,#5 						;compara o alvo com 5
	BLS salvaAlvo 					;alvo<=5, pula
	SUBS R1,R1,#1
	
salvaAlvo
	STR R1,[R0]
	
	
	
 	BX LR  
     

    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
        
        
        