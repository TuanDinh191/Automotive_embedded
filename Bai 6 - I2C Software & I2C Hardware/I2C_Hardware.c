#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C


#define I2C_SCL 	GPIO_Pin_6
#define I2C_SDA 	GPIO_Pin_7

#define I2C_GPIO 	GPIOB

typedef enum{
	NOT_OK = 0,
	OK = 1
}status;

typedef enum{
	NACK =0,
	ACK =1
}ACK_Bit;

void RCC_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_I2C1, ENABLE);
}

void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO,&GPIO_InitStructure);
}

void TIM_Config()
{
	TIM_TimeBaseInitTypeDef TIM_TimBaseInitStructure;
	TIM_TimBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimBaseInitStructure.TIM_Period = 0xFFFF;
	TIM_TimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimBaseInitStructure);
	TIM_Cmd(TIM2,ENABLE);
}
void delay_1ms()
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) <1000);
}

void delay_us(uint32_t delay)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < delay);
}

void delay_ms(uint32_t delayinms)
{
	while(delayinms)
	{
		delay_1ms();
		--delayinms;
	}
}

void I2C_Config()
{
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_OwnAddress1= 0x00;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
}

status EPROM_Write(uint16_t MemAddr, uint8_t SlaveAddress, uint8_t Numbyte, uint8_t *pData)
{
		for(int i =0; i<Numbyte; ++i)
			{
				 I2C_GenerateSTART(I2C1,ENABLE);
				 while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
				 I2C_Send7bitAddress(I2C1, (SlaveAddress<<1), I2C_Direction_Transmitter);
				 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
				 I2C_SendData(I2C1,(uint8_t)((MemAddr+i)>>8));
				 while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
				 I2C_SendData(I2C1,(uint8_t)(MemAddr+i));
				 while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
				 I2C_SendData(I2C1,pData[i]);
				 while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
				 I2C_GenerateSTOP(I2C1,ENABLE);
				 delay_ms(10);
			}
			return OK;
}

status EPROM_Read(uint16_t MemAddr, uint8_t SlaveAddress, uint8_t NumByte, uint8_t *pData)
{
	uint8_t i;
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, (SlaveAddress<<1), I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  I2C_SendData(I2C1,(MemAddr>>8));
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1,MemAddr);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, (SlaveAddress<<1), I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	for(i = 0;i< NumByte - 1;++i)
	{
	  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
		pData[i] = I2C_ReceiveData(I2C1);
	}
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	pData[i] = I2C_ReceiveData(I2C1);
	I2C_GenerateSTOP(I2C1,ENABLE);
	return OK;
}

uint8_t Data1[10] = {0x01,0x02,0x03,0x04, 0x05, 0x06, 0x07, 0x08, 0x09,0x10};
uint8_t Data2[10] = {0x10,0x09,0x08,0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
uint8_t Rcv[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int main()
{
	RCC_Config();
	TIM_Config();
	GPIO_Config();
	I2C_Config();
	while(EPROM_Write(0x0013, 0x50, 10, Data1) == NOT_OK);
	while(EPROM_Write(0x0033, 0x50, 10, Data2) == NOT_OK);
	while(1)
	{
		while(EPROM_Read(0x0013, 0x50, 10, Rcv) == NOT_OK);
		while(EPROM_Read(0x0033, 0x50, 10, Rcv) == NOT_OK);
	}
}
