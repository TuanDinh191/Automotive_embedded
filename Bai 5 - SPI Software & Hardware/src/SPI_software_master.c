#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#define SPI_SCK_Pin GPIO_Pin_8
#define SPI_MISO_Pin GPIO_Pin_9
#define SPI_MOSI_Pin GPIO_Pin_10
#define SPI_CS_Pin GPIO_Pin_11
#define SPI_GPIO GPIOD
#define SPI_RCC RCC_AHB1Periph_GPIOD

void RCC_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);

}
void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(SPI_GPIO,&GPIO_InitStruct);
}

void TIM_config(){
	TIM_TimeBaseInitTypeDef Tim_InitStruct;
	//Dem 1 lan la 1ms 
	Tim_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim_InitStruct.TIM_Prescaler = 26900-1; //1ms
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

void clock()
{
	GPIO_SetBits(SPI_GPIO, SPI_SCK_Pin);
	delay_ms(1);
	GPIO_ResetBits(SPI_GPIO, SPI_SCK_Pin);
	delay_ms(1);
}

void SPI__Init()
{
	GPIO_ResetBits(SPI_GPIO, SPI_SCK_Pin);
	GPIO_SetBits(SPI_GPIO, SPI_CS_Pin);
	GPIO_ResetBits(SPI_GPIO, SPI_MISO_Pin);
	GPIO_ResetBits(SPI_GPIO, SPI_MOSI_Pin); 
}
	
void SPI_Master_Transmit(uint8_t u8Data)
{
	uint8_t u8Mask  = 0x80;
	uint8_t tempData;
	GPIO_WriteBit(SPI_GPIO,SPI_CS_Pin, Bit_RESET);
	delay_ms(1);
	for (int i =0;i<8; ++i)
	{
		tempData = u8Data&u8Mask;
		if(tempData)
		{
			GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_SET);
			delay_ms(1);
		}
		else
		{
			GPIO_WriteBit(SPI_GPIO,SPI_MOSI_Pin,Bit_RESET);
			delay_ms(1);
		}
			u8Data <<=1;
			clock();
	}
	GPIO_WriteBit(GPIOA,SPI_CS_Pin,Bit_SET);
	delay_ms(1);      
}

uint8_t DataTrans[] = {1,3,9,10,15,19,90};
int main()
{
	RCC_Config();
	GPIO_Config();
	TIM_config();
	SPI__Init();
	while(1)
	{
		for(int i=0;i<7;i++)
			{
				SPI_Master_Transmit(DataTrans[i]);
				delay_ms(1000);
			}
	}
}



