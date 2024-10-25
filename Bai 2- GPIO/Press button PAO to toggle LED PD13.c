#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void RCC_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1ENR_GPIOAEN,ENABLE);
}
void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
void delay(__IO uint32_t timedelay)
{
	for (int i = 0; i<timedelay;++i);
}
int main()
{
	RCC_Config();
	GPIO_Config();
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)== Bit_SET)
		{
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)== Bit_SET);
			if(GPIO_ReadOutputDataBit(GPIOD,GPIO_Pin_14))
				GPIO_ResetBits(GPIOD,GPIO_Pin_14);
			else
				GPIO_SetBits(GPIOD,GPIO_Pin_14);
		}
	}
}
	
	
