#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

void RCC_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
}
void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
}

void TIM_config(){
	TIM_TimeBaseInitTypeDef Tim_InitStruct;
	//Dem 1 lan la 1 ms 
	Tim_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim_InitStruct.TIM_Prescaler = 26900-1; //1 ms
	Tim_InitStruct.TIM_Period = 0xFFFF;
	Tim_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM6,&Tim_InitStruct);
	TIM_Cmd(TIM6, ENABLE);
	//SystemCoreClock = 168000000
}
void delay_ms(uint32_t timedelay)
{
    TIM_SetCounter(TIM6, 0);              
    while (TIM_GetCounter(TIM6) < timedelay) {}   
		}
int main()
{
	RCC_Config();
	GPIO_Config();
	TIM_config();
	while(1)
	{
		//Den LED se chop tat 1s 
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		delay_ms(1000);
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);
		delay_ms(1000);
	}
}



