#include "stm32f4xx.h"

void delay(__IO uint32_t timedelay)
{
	for (int i = 0; i<timedelay;++i);
}

void toggle()
{
	if (GPIOD->ODR & (1<<13))
		GPIOD->ODR &= ~(1<<13);
	else 
		GPIOD->ODR |= (1<<13);
}
int main()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOD->OTYPER |= (~GPIO_OTYPER_ODR_13);
	GPIOD->MODER &=  ~(GPIO_MODER_MODER13_0);
	GPIOD->MODER |=   GPIO_MODER_MODER13_0;
	
	GPIOA->PUPDR &=  ~GPIO_PUPDR_PUPDR0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOA->MODER  &= ~GPIO_MODER_MODER0;
	while (1)
	{
		if ((GPIOA->IDR & (1<<0)) == 1)
		{
			while ((GPIOA->IDR & (1<<0)) == 1);
			toggle();
		}
	}
		
}

	
