#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

#define SPI_SCK_Pin GPIO_Pin_0
#define SPI_MISO_Pin GPIO_Pin_1
#define SPI_MOSI_Pin GPIO_Pin_2
#define SPI_CS_Pin GPIO_Pin_3
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA

void RCC_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(SPI_RCC,ENABLE);
}

void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_InitStruct);
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


uint8_t SPI_Slave_Receive()
{
    uint8_t dataReceive = 0x00; // Kh?i t?o d? li?u nh?n
    uint8_t bitValue;

    // Ch? d?n khi CS du?c kéo xu?ng th?p (b?t d?u truy?n)
    while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));

    // Nh?n d? li?u 8 bit t? master
    for (int i = 0; i < 8; ++i)
    {
        // Ch? c?nh lên c?a SCK d? b?t d?u d?c bit
        while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));

        // Ð?c bit t? du?ng MOSI
        bitValue = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin);

        // D?ch d? li?u nh?n sang trái và thêm bit m?i vào
        dataReceive = (dataReceive << 1) | bitValue;

        // Ch? c?nh xu?ng c?a SCK d? chu?n b? d?c bit ti?p theo
        while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
    }

    // Ch? d?n khi CS du?c kéo lên cao (k?t thúc truy?n)
    while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));

    return dataReceive;
}

void SPI_Slave_Transmit(uint8_t u8Data)
{
	uint8_t	u8Mask = 0x80;
	uint8_t tempData;
	while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	for(int i =0; i<8;++i)
	{
		while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
		tempData = u8Data & u8Mask;
		if(tempData)
		{
			GPIO_WriteBit(SPI_GPIO,SPI_MISO_Pin,Bit_SET);
			delay_ms(1);
		}else{
			GPIO_WriteBit(SPI_GPIO,SPI_MISO_Pin,Bit_RESET);
			delay_ms(1);
	}
		u8Data = u8Data<<1;
	  while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	}
	 while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
}

uint8_t DataReceive;

int main()
{
	RCC_Config();
	GPIO_Config();
	TIM_config();
	while(1) 
	{
		DataReceive = SPI_Slave_Receive();

	}
}
	
	



