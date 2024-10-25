#include "stm32f4xx.h"


void delay(__IO uint32_t timedelay)
{
	for (int i = 0; i<timedelay;++i);
}
int main()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1;
	GPIOD->OTYPER |= (~GPIO_OTYPER_ODR_14);
	GPIOD->MODER &=  ~(GPIO_MODER_MODER14_0);
	GPIOD->MODER |=   GPIO_MODER_MODER14_0;
	while (1)
	{
		GPIOD->ODR |= 1<<14;
		delay(300000);
		GPIOD->ODR &= ~(1<<14);
		delay(300000);
	}
		
}

	
