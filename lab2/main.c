// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Exercício 2 - LCD e teclado matricial: escreve "UTFPR" na primeira linha
// do display e, a cada tecla pressionada, escreve a tecla na segunda linha.
// O teclado é lido por varredura das colunas.
// Prof. Guilherme Peron

#include <stdint.h>

//variavel global para o pedestre
volatile uint8_t pedestre = 0;


void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void Timer_Init(void);
void PortK_Output(uint32_t valor);
void PortM_Output(uint32_t valor);
void LCD_Comando(uint8_t cmd);
void LCD_Dado(uint8_t c);
uint32_t PortL_Input(void);
void PortM_Dir(uint32_t valor);
void LCD_Init(uint8_t c);

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Timer_Init();
	LCD_Init();


	// Primeira linha
	LCD_Dado('U');
	LCD_Dado('T');
	LCD_Dado('F');
	LCD_Dado('P');
	LCD_Dado('R');
	LCD_Dado(' ');

	// escreve "ko-hi-" (café) em katakana
	LCD_Dado(0xBA);          // ko
	LCD_Dado(0xB0);          // - (alonga a vogal)
	LCD_Dado(0xCB);          // hi
	LCD_Dado(0xB0);          // - (alonga a vogal)

	// Segunda linha: posiciona o cursor na linha 2, coluna 1
	// As teclas pressionadas serão escritas a partir daqui
	LCD_Comando(0xC0);

	// Varredura do teclado: ativa uma coluna por vez (saída em 0) e lê as linhas.
	// Com o pull-up, as linhas ficam em 1; a linha da tecla pressionada vai para 0.
	while (1)
	{
		uint8_t linhas;

		// Coluna 1
		PortM_Dir(0x17);          // ativa a coluna 1 (PM4)
		SysTick_Wait1ms(1);       // tempo de guarda
		linhas = PortL_Input();   // lê PL0-PL3
		if(linhas != 0x0F)        // alguma tecla da coluna foi pressionada
		{
			switch (linhas)
			{
				case 0x0E:            // linha 1 (PL0)
					LCD_Dado('1');
				break;
				case 0x0D:            // linha 2 (PL1)
					LCD_Dado('4');
				break;
				case 0x0B:            // linha 3 (PL2)
					LCD_Dado('7');
				break;
				case 0x07:            // linha 4 (PL3)
					LCD_Dado('*');
				break;
			}
			SysTick_Wait1ms(200);   // debounce
		}

		// Coluna 2
		PortM_Dir(0x27);          // ativa a coluna 2 (PM5)
		SysTick_Wait1ms(1);       // tempo de guarda
		linhas = PortL_Input();   // lê PL0-PL3
		if(linhas != 0x0F)
		{
			switch (linhas)
			{
				case 0x0E:
					LCD_Dado('2');
				break;
				case 0x0D:
					LCD_Dado('5');
				break;
				case 0x0B:
					LCD_Dado('8');
				break;
				case 0x07:
					LCD_Dado('0');
				break;
			}
			SysTick_Wait1ms(200);   // debounce
		}

		// Coluna 3
		PortM_Dir(0x47);          // ativa a coluna 3 (PM6)
		SysTick_Wait1ms(1);       // tempo de guarda
		linhas = PortL_Input();   // lê PL0-PL3
		if(linhas != 0x0F)
		{
			switch (linhas)
			{
				case 0x0E:
					LCD_Dado('3');
				break;
				case 0x0D:
					LCD_Dado('6');
				break;
				case 0x0B:
					LCD_Dado('9');
				break;
				case 0x07:
					LCD_Dado('#');
				break;
			}
			SysTick_Wait1ms(200);   // debounce
		}

		// Coluna 4
		PortM_Dir(0x87);          // ativa a coluna 4 (PM7)
		SysTick_Wait1ms(1);       // tempo de guarda
		linhas = PortL_Input();   // lê PL0-PL3
		if(linhas != 0x0F)
		{
			switch (linhas)
			{
				case 0x0E:
					LCD_Dado('A');
				break;
				case 0x0D:
					LCD_Dado('B');
				break;
				case 0x0B:
					LCD_Dado('C');
				break;
				case 0x07:
					LCD_Dado('D');
				break;
			}
			SysTick_Wait1ms(200);   // debounce
		}
	}

}

// -------------------------------------------------------------------------------
// Função LCD_Comando
// Envia uma instrução para o LCD (RS = 0)
// Parâmetro de entrada: código do comando
// Parâmetro de saída: não tem
void LCD_Comando(uint8_t cmd)
{
	PortK_Output(cmd);       // 1. coloca o comando no barramento de dados
	PortM_Output(0x04);      // 2. EN = 1, RS = 0
	SysTick_Wait1us(10);
	PortM_Output(0x00);      // 3. EN = 0
	SysTick_Wait1us(40);     // tempo para o LCD executar o comando
}

// -------------------------------------------------------------------------------
// Função LCD_Dado
// Envia um caractere ASCII para o LCD (RS = 1)
// Parâmetro de entrada: caractere a ser escrito
// Parâmetro de saída: não tem
void LCD_Dado(uint8_t c)
{
	PortK_Output(c);         // 1. coloca o caractere no barramento de dados
	PortM_Output(0x05);      // 2. EN = 1, RS = 1
	SysTick_Wait1us(10);
	PortM_Output(0x00);      // 3. EN = 0
	SysTick_Wait1us(40);     // tempo para o LCD escrever o caractere
}


// -------------------------------------------------------------------------------
// Função LCD_Init
// Envia um caractere ASCII para o LCD (RS = 1)
// Parâmetro de entrada: caractere a ser escrito
// Parâmetro de saída: não tem

void LCD_Init(uint8_t c)
{
	// Inicialização do LCD
	LCD_Comando(0x38);       // modo 8 bits, 2 linhas, matriz 5x7
	LCD_Comando(0x06);       // cursor com autoincremento para a direita
	LCD_Comando(0x0E);       // liga o display e o cursor, sem piscar
	LCD_Comando(0x01);       // limpa o display e leva o cursor para o home
	SysTick_Wait1us(1640);   // o comando 0x01 precisa de 1,64ms
}
