#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#define SPI1_NSS GPIO_Pin_4
#define SPI1_SCK GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7
#define SPI1_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA

void RCC_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(SPI_RCC|RCC_APB2Periph_SPI1,ENABLE);
}

void GPIO_Config()
{
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = SPI1_NSS | SPI1_SCK | SPI1_MISO | SPI1_MOSI;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
}

void TIM_config(){
	TIM_TimeBaseInitTypeDef Tim_InitStruct;
	//Dem 1 lan la 0.1ms 
	Tim_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim_InitStruct.TIM_Prescaler = 7200 -1;
	Tim_InitStruct.TIM_Period = 0xFFFF;
	Tim_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2,&Tim_InitStruct);
	TIM_Cmd(TIM2, ENABLE);
	//SystemCoreClock
}

void delay_ms(uint32_t timedelay)
{
	TIM_SetCounter(TIM2, 0);
// Ð?t giá tr? ban d?u cho timer
  while(TIM_GetCounter(TIM2)< timedelay*10){}
// L?y giá tr? d?m hi?n t?i c?a timer
}

void SPI_Config()
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16 ; //stm32f4hard master
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

uint8_t SPI_Receive1Byte()
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);
	uint8_t temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);

	

	return temp;
}

void SPI_Send1Byte(uint8_t data)
{
	//delay_ms(1);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, (uint16_t)data);
	//delay_ms(1);
}

uint8_t DataReceive;

int main()
{
 	RCC_Config();
	GPIO_Config();
	TIM_config();
	SPI_Config();
	while(1){
		while(GPIO_ReadInputDataBit(SPI1_GPIO, SPI1_NSS) == 1);
		if(GPIO_ReadInputDataBit(GPIOA,SPI1_NSS) == 0){
		  DataReceive = SPI_Receive1Byte();
		}
	}
}
	

