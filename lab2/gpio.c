// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas K, L e M, usadas pelo LCD e pelo teclado matricial,
// a porta N (LED PN0) e o Timer 2
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

extern volatile uint16_t segundos;   // criada no main.c
extern volatile uint8_t contando;   

// Contador de milissegundos: somado na interrupção e zerado no início de cada partida
static volatile uint16_t ms = 0;

#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTF  (0x0020)


#define GPIO_PORTK  (0x0200) //bit 9
#define GPIO_PORTM  (0x0800) //bit 11
#define GPIO_PORTL  (0x0400) //bit 10

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports K (barramento de dados D7-D0 do LCD),
// L (linhas do teclado) e M (RS, R/W e EN do LCD e colunas do teclado)
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTN);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
	while((SYSCTL_PRGPIO_R & (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTN) ) != (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTN) ){};

	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;

	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTK_DIR_R = 0xFF;   //PK0 a PK7 como saída
	GPIO_PORTM_DIR_R = 0x07;   //PM0 a PM2 como saída; PM4 a PM7 começam como entrada (alta impedância)
	GPIO_PORTL_DIR_R = 0xF0;   //PL0 a PL3 como entrada
	GPIO_PORTN_DIR_R = 0x01;   //PN0 (LED) como saída

	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;

	// 6. Setar os bits de DEN para habilitar I/O digital
	GPIO_PORTK_DEN_R = 0xFF;   //PK0 a PK7
	GPIO_PORTM_DEN_R = 0xF7;   //PM0 a PM2 e PM4 a PM7
	GPIO_PORTL_DEN_R = 0x0F;   //PL0 a PL3
	GPIO_PORTN_DEN_R = 0x01;   //PN0

	// 7. Habilitar resistor de pull-up interno nas linhas do teclado, setar PUR para 1
	GPIO_PORTL_PUR_R = 0x0F;   //PL0 a PL3
}

// -------------------------------------------------------------------------------
// Função PortM_Output
// Escreve os valores nos pinos PM0 (RS), PM1 (R/W) e PM2 (EN)
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortM_Output(uint32_t valor)
{
    const uint32_t mascara_lcd = 0x07u;
    uint32_t dados = GPIO_PORTM_DATA_R;

    dados &= ~mascara_lcd;          // Limpa PM0–PM2 na cópia
    dados |= valor & mascara_lcd;   // Insere os novos bits do LCD
    GPIO_PORTM_DATA_R = dados;
}

// -------------------------------------------------------------------------------
// Função PortL_Input
// Lê as linhas do teclado (PL0 a PL3)
// Parâmetro de entrada: Não tem
// Parâmetro de saída: valor das linhas (bit em 0 = tecla pressionada naquela linha)
uint32_t PortL_Input(void)
{
	return GPIO_PORTL_DATA_R & 0x0F;
}
// -------------------------------------------------------------------------------
// Função PortK_Output
// Escreve o comando ou caractere no barramento de dados (PK0 a PK7)
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortK_Output(uint32_t valor)
{
    uint32_t temp;
    //os 8 bits do port são usados, então não há bits a preservar
    temp = GPIO_PORTK_DATA_R & 0x00;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTK_DATA_R = temp;
}


// -------------------------------------------------------------------------------
// Seleciona um pino de PM4 a PM7 como saida baixa para a varredura do teclado.
// Os demais pinos do teclado ficam como entradas de alta impedancia.
void PortM_SelecionaColuna(uint32_t coluna)
{
    GPIO_PORTM_DIR_R = (GPIO_PORTM_DIR_R & ~0xF0u) | coluna;
    GPIO_PORTM_DATA_R &= ~coluna;
}


// -------------------------------------------------------------------------------
// Função Timer_Init
// Configura o Timer 2A no modo periódico (32 bits) para gerar uma interrupção
// a cada 1 ms (tratada em Timer2A_Handler).
//
// Cálculo da contagem, com clock de 80 MHz (PLL):
// X = 1 ms × 80 MHz = 0,001 s × 80.000.000 Hz = 80.000 ticks
// contagem = X - 1 = 79.999
//   16 bits conta até 65.536: não basta, então usamos 32 bits (sem prescale)
//   contagem = 80.000 - 1 = 79.999
void Timer_Init(void)
{
	// 1. Como vamos utilizar o timer2 , habilitar o bit 2 no registrador RCGCTIMER e depois esperar
	// enquanto bit 2 do registrador PRTIMER não está setado
	SYSCTL_RCGCTIMER_R |= 0x04;
	while((SYSCTL_PRTIMER_R & 0x04) == 0){};

	// 2. Desabilitar o timer2 para fazer as configurações (adiante vamos habilitá lo novamente)
	TIMER2_CTL_R = 0x00;

	// 3.Conforme calculamos os valores, colocar o timer2 no modo 32 bits então escrever 0x00 nos
	// bits respectivos do registrador GPTMCFG
	TIMER2_CFG_R = 0x00;

	// 4. Como vamos usar o timerA e queremos uma interrupção a cada 1 ms, colocá-lo no modo
	// periódico (TAMR = 0x02; one-shot seria 0x01)
	TIMER2_TAMR_R = 0x02;

	// 5. Carregar o valor de contagem no registrador
  // timerA no registrador GPTMTAILR
	TIMER2_TAILR_R = 79999; //1 ms

	// 6. Prescale = 0: o prescaler só existe no modo 16 bits
	TIMER2_TAPR_R = 0x00;

	// 7. Como vamos utilizar o timerA , setar o bit TnTOCINT no registrador GPTMICR , para
	//garantir que a primeira interrupção seja atendida.
	TIMER2_ICR_R = 0x01;

	// 8a. Habilitar a interrupção de time-out do TimerA (TATOIM = 1 no GPTMIMR)
	TIMER2_IMR_R |= 0x01;

	// 8b. Prioridade 4 no NVIC. O Timer2A é a interrupção nº 23 (tabela 2-9 do datasheet);
	//     o PRI5 guarda as interrupções 20 a 23 e a 23 fica nos bits 31:29
	NVIC_PRI5_R = 4 << 29;

	// 8c. Habilitar a interrupção nº 23 no NVIC (bit 23 do EN0, que cobre as interrupções 0 a 31)
	NVIC_EN0_R = 1 << 23;

	// 9. Habilitar o timer (TAEN = 1 no GPTMCTL): começa a contar a partir de agora.
	//    No modo periódico ele recarrega o TAILR sozinho ao chegar em zero
	TIMER2_CTL_R =  TIMER2_CTL_R | 0x01;
}

// -------------------------------------------------------------------------------
// 10. Rotina de interrupção do Timer 2A (nome definido no startup.s)
// Chamada a cada 1 ms. A cada 1000 chamadas, soma 1 segundo ao tempo de jogo,
// se o tempo estiver correndo
void Timer2A_Handler(void)
{
	TIMER2_ICR_R = 0x01;         // limpa o flag de interrupção

	ms += 1;
 
	if (ms == 1000) // passou 1 segundo
	{
		ms=0;

		if (contando==1) // só conta se o jogo estiver em andamento
		{
			segundos += 1;
		}
	}
}

// -------------------------------------------------------------------------------
// Função Timer_Valor
// Devolve o valor atual da contagem do Timer 2A (registrador GPTMTAR)
// Usado no sorteio
// Parâmetro de saída: valor atual do timer (0 a 79.999)
uint32_t Timer_Valor(void)
{
		return TIMER2_TAR_R;
}

// -------------------------------------------------------------------------------
// Função Timer_ZeraMs
// Zera o contador de milissegundos, para o 1º segundo da partida durar 1 s inteiro
// Parâmetro de entrada: não tem
// Parâmetro de saída: não tem
void Timer_ZeraMs(void)
{
	ms = 0;
}
