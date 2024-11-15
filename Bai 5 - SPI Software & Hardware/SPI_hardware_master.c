#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"

#define SPI1_NSS  GPIO_Pin_4
#define SPI1_SCK  GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7
#define SPI1_GPIO GPIOA
#define SPI1_RCC  RCC_AHB1Periph_GPIOA
void RCC_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	RCC_AHB1PeriphClockCmd(SPI1_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
}

void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_NSS | SPI1_MISO | SPI1_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_PinAFConfig(SPI1_GPIO,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_GPIO,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_GPIO,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_GPIO,GPIO_PinSource7,GPIO_AF_SPI1);
	GPIO_Init(SPI1_GPIO,&GPIO_InitStructure);

	
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

void SPI_Config()
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
}

void SPI_Send1Byte(uint8_t data)
{
	GPIO_ResetBits(GPIOA, SPI1_NSS);
	delay_ms(1000);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, (uint16_t)data);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);

	GPIO_SetBits(GPIOA, SPI1_NSS);
	delay_ms(1000);
}


uint8_t DataTrans[] = {1,3,9,10,15,19,70};
int main()
{
	RCC_Config();
	GPIO_Config();
	TIM_config();
	SPI_Config();
	while(1)
	{
		for(int i=0;i<7;i++)
			{
				SPI_Send1Byte(DataTrans[i]);
				delay_ms(1000);
			}
			
	}
}

