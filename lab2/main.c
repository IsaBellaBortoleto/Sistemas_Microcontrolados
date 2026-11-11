// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Lab 2 (jogo de adivinhação)
// Prof. Guilherme Peron

#include <stdint.h>

//variavel global para os segundos
volatile uint16_t segundos = 0;
// 1 = tempo correndo, 0 = parado
volatile uint8_t contando = 0;   


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
void LCD_Limpa(void);
void tela_digitando(void);
uint8_t Teclado_Le(void);
void estado_init_func(uint8_t tecla);
void estado_digitando_func(uint8_t tecla);
void estado_dica_func(uint8_t tecla);
void estado_acertou_func(uint8_t tecla);
void tela_dica(void);
void tela_acertou(void);
uint32_t Timer_Valor(void);            

typedef enum estJogo
{
	ESTADO_INICIO,
	ESTADO_DIGITANDO,
	ESTADO_MOSTRANDO_DICA,
	ESTADO_ACERTOU
}estadosJogo;

estadosJogo estado = ESTADO_INICIO;
uint8_t num_secreto;
uint8_t palpite=0;
uint8_t tentativas=0;
uint8_t qtd_digitos = 0;
int main(void)
{
	uint8_t tecla;

	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Timer_Init();
	LCD_Init();
	LCD_Escreve(1, 0, "Adivinhe: 0-99");

	while (1)
	{
		tecla=Teclado_Le();
		if(tecla != 0)
		{
			switch(estado)
			{
				case ESTADO_INICIO:
					estado_init_func(tecla);
				break;
				case ESTADO_DIGITANDO:
					estado_digitando_func(tecla);
				break;
				case ESTADO_MOSTRANDO_DICA:
					estado_dica_func(tecla);
				break;
				case ESTADO_ACERTOU:
					estado_acertou_func(tecla);
				break;
			}
		}
	}

}
// -------------------------------------------------------------------------------
// Função Teclado_Le
// Faz a varredura do teclado matricial 4x4, uma coluna por vez
uint8_t Teclado_Le(void)
{
		uint8_t linhas;
		uint8_t tecla = 0;
	
		// Coluna 1
		PortM_Dir(0x17);          // ativa a coluna 1 (PM4)
		SysTick_Wait1ms(1);       // tempo de guarda
		linhas = PortL_Input();   // lê PL0-PL3
		if(linhas != 0x0F)        // alguma tecla da coluna foi pressionada
		{
			switch (linhas)
			{
				case 0x0E:            // linha 1 (PL0)
					tecla ='1';
				break;
				case 0x0D:            // linha 2 (PL1)
					tecla ='4';
				break;
				case 0x0B:            // linha 3 (PL2)
					tecla ='7';
				break;
				case 0x07:            // linha 4 (PL3)
					tecla ='*';
				break;
			}
			while (PortL_Input() != 0x0F) {}   // espera a tecla ser solta, se n precisar, remover
			SysTick_Wait1ms(200);   // debounce
			return tecla;
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
					tecla='2';
				break;
				case 0x0D:
					tecla='5';
				break;
				case 0x0B:
					tecla='8';
				break;
				case 0x07:
					tecla='0';
				break;
			}
			while (PortL_Input() != 0x0F) {}
			SysTick_Wait1ms(200);   // debounce
			return tecla;

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
					tecla='3';
				break;
				case 0x0D:
					tecla='6';
				break;
				case 0x0B:
					tecla='9';
				break;
				case 0x07:
					tecla='#';
				break;
			}
			while (PortL_Input() != 0x0F) {}
			SysTick_Wait1ms(200);   // debounce
			return tecla;

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
					tecla='A';
				break;
				case 0x0D:
					tecla='B';
				break;
				case 0x0B:
					tecla='C';
				break;
				case 0x07:
					tecla='D';
				break;
			}
			while (PortL_Input() != 0x0F) {}
			SysTick_Wait1ms(200);   // debounce
			return tecla;

		}
		return 0;
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
	SysTick_Wait1ms(15);     // espera o LCD terminar de ligar (>15 ms após energizar)
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

// -------------------------------------------------------------------------------
// Função LCD_Limpa
// limpa o display 
void LCD_Limpa(void)
{
	LCD_Comando(0x01);       // limpa o display e leva o cursor para o home
	SysTick_Wait1us(1640);   // o comando 0x01 precisa de 1,64ms
}

// -------------------------------------------------------------------------------
// Função tela_digitando
// desenha a tela do estado DIGITANDO: palpite na linha 1 e tentativas na linha 2
void tela_digitando(void)
{
	LCD_Limpa();    
	LCD_Escreve(1,0,"Palpite: ");
	LCD_Numero(1,9,palpite,2);
	LCD_Escreve(2,0,"Tentativas: ");
	LCD_Numero(2,12,tentativas,2);
}

// -------------------------------------------------------------------------------
// Função estado_init_func
// Qualquer tecla: sorteia o número secreto (Timer % 100) e zera tentativas e tempo
void estado_init_func(uint8_t tecla)
{
	
	//trocar depois
	num_secreto = Timer_Valor()%100;
	segundos = 0;
	palpite = 0;
	tentativas = 0;
	qtd_digitos = 0;
	contando = 1;
	tela_digitando();
	estado = ESTADO_DIGITANDO;
}

// -------------------------------------------------------------------------------
// Função estado_digitando_func
// Trata as teclas no estado DIGITANDO: dígitos montam o palpite,
// * apaga e # confirma o palpite
// Parâmetro de entrada: tecla pressionada
void estado_digitando_func(uint8_t tecla)
{
	if (tecla >= '0' && tecla <= '9')
	{
		if (qtd_digitos < 2)
		{
			palpite = palpite * 10 + (tecla - '0');
			qtd_digitos += 1;
			tela_digitando();		
		}
	}
	else if (tecla == '*')
	{
		palpite=0;
		qtd_digitos=0;
		tela_digitando();
	}
	else if (tecla == '#')
	{
		tentativas += 1;
		if (palpite == num_secreto)
		{
			contando = 0;
			estado = ESTADO_ACERTOU;
			tela_acertou();
		}
		else
		{
			tela_dica();
			palpite=0;
			qtd_digitos=0;
			estado = ESTADO_MOSTRANDO_DICA;
		}
	}
	
}

// -------------------------------------------------------------------------------
// Função estado_dica_func
// Trata as teclas no estado MOSTRANDO_DICA
// Parâmetro de entrada: tecla pressionada
void estado_dica_func(uint8_t tecla)
{
	if (tecla >= '0' && tecla <= '9')
	{
		palpite = tecla - '0';    // caractere -> número
		qtd_digitos = 1;          // o novo palpite já tem 1 dígito
		estado = ESTADO_DIGITANDO;
		tela_digitando();
	}
	else if (tecla == '*')
	{
		estado = ESTADO_DIGITANDO;
		tela_digitando();
	}
}

// -------------------------------------------------------------------------------
// Função estado_acertou_func
// Trata as teclas no estado ACERTOU: só a tecla A faz algo
// Parâmetro de entrada: tecla pressionada
void estado_acertou_func(uint8_t tecla)
{

	if (tecla == 'A')
	{
		estado_init_func(tecla);   // nova partida: mesmo início da primeira
	}
}

// -------------------------------------------------------------------------------
// Função tela_dica
// Desenha a tela do estado MOSTRANDO_DICA
void tela_dica(void)
{
	LCD_Limpa();       
	LCD_Escreve(1,0,"Palpite: ");
	LCD_Numero(1,9,palpite,2);
	LCD_Escreve(2,0,"Dica: ");
	if(palpite<num_secreto)
	{
		LCD_Escreve(2,6,"MAIOR");
	}
	else
	{
		LCD_Escreve(2,6,"MENOR");
	}		
}

// -------------------------------------------------------------------------------
// Função tela_acertou
// Desenha a tela do estado ACERTOU número secreto na linha 1,
// tentativas e tempo total na linha 2
void tela_acertou(void)
{
	LCD_Limpa();
	LCD_Escreve(1,0,"ACERTOU! N=");
	LCD_Numero(1,11,num_secreto,2);
	LCD_Escreve(2,0,"Tent:");
	LCD_Numero(2,5,tentativas,2);
	LCD_Escreve(2,8,"T:");
	LCD_Numero(2,10,segundos,3);
	LCD_Escreve(2,13,"s");
}