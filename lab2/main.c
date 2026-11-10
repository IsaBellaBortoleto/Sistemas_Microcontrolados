// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Lab 2 (jogo de adivinhação)
// Prof. Guilherme Peron

#include <stdint.h>



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
void LCD_Init(void);
void LCD_Cursor(uint8_t linha,uint8_t coluna);
void LCD_Escreve(uint8_t linha, uint8_t coluna, const char *texto);
void LCD_Numero(uint8_t linha,uint8_t coluna,uint16_t valor,uint8_t digitos);

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Timer_Init();
	LCD_Init();
	LCD_Escreve(1, 0, "Adivinhe: 0-99");




	// Segunda linha: posiciona o cursor na linha 2, coluna 1
	// As teclas pressionadas serão escritas a partir daqui
	LCD_Comando(0xC0);

	// Varredura do teclado: ativa uma coluna por vez (saída em 0) e lê as linhas
	// Com o pull-up, as linhas ficam em 1; a linha da tecla pressionada vai para 0
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
// Inicialização do LCD
void LCD_Init(void)
{
	LCD_Comando(0x38);       // modo 8 bits, 2 linhas, matriz 5x7
	LCD_Comando(0x06);       // cursor com autoincremento para a direita
	LCD_Comando(0x0E);       // liga o display e o cursor, sem piscar
	LCD_Comando(0x01);       // limpa o display e leva o cursor para o home
	SysTick_Wait1us(1640);   // o comando 0x01 precisa de 1,64ms
}

// -------------------------------------------------------------------------------
// Função LCD_Cursor
// Posicionar o cursor do LCD
//Parâmetros de entrada: linha (1 ou 2) e coluna (0 a 15)
void LCD_Cursor(uint8_t linha,uint8_t coluna)
{
	uint8_t cursor=0;
	if (linha == 1)
	{
		cursor = coluna + 0x80;
	}
	else
	{
		cursor = coluna + 0x80 + 0x40;
	}
	LCD_Comando(cursor);

}


// -------------------------------------------------------------------------------
// Função LCD_Escreve
// escrever uma string numa posição
//Parâmetros de entrada: linha (1 ou 2) e coluna (0 a 15) e texto
void LCD_Escreve(uint8_t linha, uint8_t coluna, const char *texto)
{
	uint8_t i=0;
	LCD_Cursor(linha,coluna);
	while(texto[i] != '\0')
	{
			LCD_Dado(texto[i]);
			i++;
	}
	
}

// -------------------------------------------------------------------------------
// Função LCD_Numero
// escreve um número de 2 ou 3 dígitos numa posição
//Parâmetros de entrada:linha, coluna, valor (0 a 999) e digitos (2 ou 3)
void LCD_Numero(uint8_t linha,uint8_t coluna,uint16_t valor,uint8_t digitos)
{
	LCD_Cursor(linha, coluna);

	if (digitos == 3)
	{
			// centena
			LCD_Dado((valor/100) + '0');
	}

	// dezena
	LCD_Dado((valor%100)/10 + '0');

	LCD_Dado((valor%10) + '0');   // unidade
	
}