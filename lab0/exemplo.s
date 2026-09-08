; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>

VET_IN EQU 0x20000400 ;vetor de entrada
VET_OUT EQU 0x20000600 ;vetor de saída (lista de palindromos) 
	
; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

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

; -------------------------------------------------------------------------------



; Função main()
Start  
; Comece o código aqui <======================================================

;-------------------------------------------------------------------------------
; Inicializacao
;-------------------------------------------------------------------------------
    LDR   R0, =VET_IN
    LDR   R1, =VET_OUT
	
	MOV R2,#0 ;contador de numeros ja lidos
	MOV R3,#0 ;contador de palindromos
	MOV R9,#10 ;cte 10
	
	BL varre_laco
	
	BL bubble_sort
	
fim
	NOP
	B fim
	
;-------------------------------------------------------------------------------
; Varredura do vetor de entrada
;-------------------------------------------------------------------------------
varre_laco
	CMP R2,#30 
	BHS varre_fim ;contador >= 30?
	LDRH R4,[R0], #2 ;le e avanca
	ADDS R2,R2,#1 ; incrementa o contador
	
;-------------------------------------------------------------------------------
; Inversao dos digitos
; Entrada: R4 = numero original
; Saida: R8 = numero com os digitos invertidos
;-------------------------------------------------------------------------------
	MOV R5,R4
	MOV R8,#0
inv_laco
	CMP R5,#0
	BEQ fim_inv ; acabaram os digitos
	UDIV R6,R5,R9 ;R6=R5/10 - remove último dígito
	MLS R7,R6,R9,R5 ;R7=R5-R6*10 - pega o último dígito
	MLA R8,R8,R9,R7 ;R8=R8*10+R7 - número invertido
	MOV R5,R6
	B inv_laco
fim_inv

;-------------------------------------------------------------------------------
; Adiciona ou nao a lista de palindromos
;-------------------------------------------------------------------------------
	CMP R8,R4
	BNE proximo_num  ;nao e palindromo: pula a gravacao
	ADDS R3,R3,#1
	STRH R4,[R1],#2 ;grava e avanca o ponteiro de escrita
proximo_num
	B varre_laco
varre_fim
	BX LR
	
bubble_sort
	SUB R2, R3, #1 ;R3 guarda o tamanho do vetor, R2 eh simplesmente R3-1 para ser o numero de comparacoes do loop2
loop1
	LDR R0, =VET_OUT
	MOV R1, #0
	MOV R7, #0 ;indica se ouve troca
loop2
	LDRH R4, [R0], #2
	LDRH R5, [R0]
	CMP R4, R5
	BLE nao_troca
	MOV R6, R4
	MOV R4, R5
	MOV R5, R6
	STRH R4, [R0, #-2] 
	STRH R5, [R0]
	MOV R7, #1
nao_troca
	ADD R1, R1, #1
	CMP R1, R2
	BNE loop2
	CMP R7, #0
	BNE loop1
	
	BX LR

    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
