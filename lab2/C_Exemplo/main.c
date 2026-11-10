// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>

//variavel global para o pedestre
volatile uint8_t pedestre = 0;


void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortF_Output(uint32_t leds);
void Pisca_leds(void);
void estado_init_func(void);
void estado_vmvd_func(void);
void estado_vmam_func(void);
void estado_vmvm_func(void);
void estado_vdvm_func(void);
void estado_vdvm_func(void);
void estado_amvm_func(void);
void estado_pedestre_func(void);

typedef enum estSemaforo
{
	ESTADO_INIT,
	ESTADO_VMVD,
	ESTADO_VMAM,
	ESTADO_VMVM,
	ESTADO_VDVM,
	ESTADO_AMVM,
	ESTADO_PEDESTRE
}estadosSemaforo;


estadosSemaforo estados = ESTADO_INIT;
int main(void)
{

	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	while (1)
	{
		
		
			switch(estados)
		{
			case ESTADO_INIT:
				estado_init_func();
			break;
			case ESTADO_VMVD:
				estado_vmvd_func();
			break;
			case ESTADO_VMAM:
				estado_vmam_func();
			break;
			case ESTADO_VMVM:
				estado_vmvm_func();
			break;
			case ESTADO_VDVM:
				estado_vdvm_func();
			break;
			case ESTADO_AMVM:
				estado_amvm_func();
			break;
			case ESTADO_PEDESTRE:
				estado_pedestre_func();
			break;
		}
	}
	

		
			
//    //Se a USR_SW2 estiver pressionada
//		if (PortJ_Input() == 0x1)
//			PortN_Output(0x1);
//    //Se a USR_SW1 estiver pressionada
//		else if (PortJ_Input() == 0x2)
//			PortN_Output(0x2);
//    //Se ambas estiverem pressionadas
//		else if (PortJ_Input() == 0x0)
//			Pisca_leds();
//    //Se nenhuma estiver pressionada
//		else if (PortJ_Input() == 0x3)
//			PortN_Output(0x0);        
	
}

//semaforo 1 - vermelho
//semaforo 2 - vermelho
//por 2s
//4 leds acesos
void estado_init_func(void) 
{
	PortN_Output(0x3);//vm
	PortF_Output(0x11);//vm
	SysTick_Wait1ms(2000); //2000ms
	estados = ESTADO_VMVD;
}

//semaforo 1 - vermelho
//semaforo 2 - verde
//por 20s
//3 led acesos
void estado_vmvd_func(void)
{
	PortN_Output(0x3);//vm
	
	PortF_Output(0x10);//vd
	SysTick_Wait1ms(20000); //20000ms
	estados = ESTADO_VMAM;
}

//semaforo 1 - vermelho
//semaforo 2 - amarelo
//por 4s
//3 led acesos
void estado_vmam_func(void)
{
	//vm
	PortN_Output(0x3);
	
	PortF_Output(0x1);//am
	SysTick_Wait1ms(4000); //4000ms
	estados = ESTADO_VMVM;
}



//semaforo 1 - vermelho
//semaforo 2 - vermelho
//por 2s
//4 led acesos
void estado_vmvm_func(void)
{
	PortN_Output(0x3);//vm
	
	PortF_Output(0x11);//vm
	SysTick_Wait1ms(2000); //2000ms
	estados = ESTADO_VDVM;
}

//semaforo 1 - verde
//semaforo 2 - vermelho
//por 20s
//3 led acesos
void estado_vdvm_func(void)
{
	PortN_Output(0x2); //vd
	PortF_Output(0x11); //vm

	SysTick_Wait1ms(20000); //20000ms
	estados = ESTADO_AMVM;
}

//semaforo 1 - amarelo
//semaforo 2 - vermelho
//por 4s
//3 led acesos
void estado_amvm_func(void)
{
	PortN_Output(0x1); //am
	
	PortF_Output(0x11); //vm

	SysTick_Wait1ms(4000); //4000ms
	
	
	//Se a USR_SW1 estiver pressionada em algum momento do ciclo
	if (pedestre == 1)
	{
		estados = ESTADO_PEDESTRE;
	}
	//Se a USR_SW1 nao estiver pressionada em algum momento do ciclo
	else
	{
		estados = ESTADO_INIT;
	}
}

//semaforo 1 - vermelho
//semaforo 2 - vermelho
//por 20s
//4 led acesos
void estado_pedestre_func(void)
{
	PortN_Output(0x3);//vm
	PortF_Output(0x11);//vm
	SysTick_Wait1ms(20000); //espera 20 seg
	pedestre=0;
	estados = ESTADO_INIT;
}


		

//void Pisca_leds(void)
//{
//	PortN_Output(0x2);
//	SysTick_Wait1ms(250);
//	PortN_Output(0x1);
//	SysTick_Wait1ms(250);
//}
