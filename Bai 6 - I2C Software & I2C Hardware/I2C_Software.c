#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

#define I2C_SCL		GPIO_Pin_1
#define I2C_SDA   GPIO_Pin_2

#define I2C_GPIO  GPIOA

#define WRITE_SDA_0 GPIO_ResetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SDA_1 GPIO_SetBits(I2C_GPIO,I2C_SDA)
#define WRITE_SCL_0 GPIO_ResetBits(I2C_GPIO,I2C_SCL)
#define WRITE_SCL_1 GPIO_SetBits(I2C_GPIO,I2C_SCL)
#define READ_SDA_VAL GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA)

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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
void 	GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
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
	WRITE_SDA_1;
	delay_us(1);
	WRITE_SCL_1;
	delay_us(1);
}
void I2C_Start()
{
	WRITE_SDA_1; // Set SDA,SCL to 1: ideal status
	delay_us(3);
	WRITE_SCL_1;
	delay_us(3);
	WRITE_SDA_0;	// SDA reset to 0 before SCL
	delay_us(3);	
	WRITE_SCL_0;
	delay_us(3);
}

void I2C_Stop()
{
	WRITE_SDA_0;
	delay_us(3);
	WRITE_SCL_1;
	delay_us(3);
	WRITE_SDA_1;
	delay_us(3);
}

status I2C_Write(uint8_t pData) //Write a byte data, return the status of writing
{
	status stRet;
	for (int i =0; i<8; ++i)
		{
			if (pData & 0x80)
				WRITE_SDA_1;
			else 
				WRITE_SDA_0;
			
			delay_us(3); // Make sure SDA high complete
		
			//SCL clock
			WRITE_SCL_1;
			delay_us(5);
			WRITE_SCL_0;
			delay_us(5);
			
			pData <<=1;
		}
		
	 WRITE_SDA_1; //Set SDA to read ACK from slave
	 delay_us(3);
	 WRITE_SCL_1;  //Set scl to 1 at 3ms make sure to read stable data at the middle pulse high
	 delay_us(3);
		
	 if(READ_SDA_VAL)
		 stRet = NOT_OK;
	 else
		 stRet = OK;
	  
	 delay_us(2);
	 WRITE_SCL_0;
	 delay_us(5);
	 
	 return stRet;
		
}
uint8_t I2C_Read(ACK_Bit _ACK)
{ 
	uint8_t u8Ret = 0x00;
	WRITE_SDA_1;
	delay_us(1);
	for (int i =0;i <8;++i)
	{
		u8Ret <<= 1;
		//SCL clock
		WRITE_SCL_1;
		delay_us(3);
		if(READ_SDA_VAL){
			u8Ret |= 0x01;
		}
		delay_us(2);
		WRITE_SCL_0;
		delay_us(5);
	}
	if(_ACK)
		WRITE_SDA_0;
	else 
		WRITE_SDA_1;
	
	delay_us(1);
	
	WRITE_SCL_1;
	delay_us(5);
	WRITE_SCL_0;
	delay_us(5);
	
	return u8Ret; 
}
status EPROM_Write(uint16_t MemAddr, uint8_t SlaveAddress, uint8_t NumByte, uint8_t *pData)
{
	for (int i =0;i<NumByte;++i)
	{
	I2C_Start();
	if (I2C_Write(SlaveAddress<<1) == NOT_OK)
	{
		I2C_Stop();
		return NOT_OK;
	}
	if (I2C_Write((MemAddr+i)>>8) == NOT_OK)
	{
	  I2C_Stop();
		return NOT_OK;
	}
	if (I2C_Write(MemAddr+i) == NOT_OK)
	{
		I2C_Stop();
		return NOT_OK;
	}
	if (I2C_Write(pData[i]) == NOT_OK)
	{	
		I2C_Stop();
		return NOT_OK;
	}
		I2C_Stop();
		delay_ms(10);
	}
	return OK;
}
status EPROM_Read(uint16_t MemAddr, uint8_t SlaveAddress, uint8_t NumByte, uint8_t *pData)
{
	uint8_t i;
	I2C_Start();
	if(I2C_Write(SlaveAddress<<1 ) == NOT_OK) //Write the address of eprom 
		{
			I2C_Stop();
			return NOT_OK;
		}
	if (I2C_Write(MemAddr >> 8) == NOT_OK)
		{
			I2C_Stop();
			return NOT_OK;
		}
	if (I2C_Write(MemAddr) == NOT_OK)
		{
			I2C_Stop();
			return NOT_OK;
		}
		
	I2C_Start();
	
	if (I2C_Write((SlaveAddress << 1) | 1) == NOT_OK)
	{
		I2C_Stop();
		return NOT_OK;
	}
	for (i =0;i <NumByte -1; ++i)
		{
			pData[i] = I2C_Read(ACK);
		}
	
	pData[i] = I2C_Read(NACK);
	I2C_Stop();
	return OK;
}

uint8_t Data1[10] = {0x03,0x05,0x12,0xEC, 0xDE, 0x28, 0xAB, 0xBD, 0x22,0x55};
uint8_t Data2[10] = {0x01,0x04,0x35,0xCC, 0xEE, 0xFF, 0xCA, 0x81, 0x74, 0x12};
uint8_t Rcv[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int main()
{
	RCC_Config();
	TIM_Config();
	GPIO_Config();
	I2C_Config();
	while(EPROM_Write(0x0013, 0x50, 10,Data1) == NOT_OK);
	while(EPROM_Write(0x0033, 0x50, 10,Data2) == NOT_OK);
	while(1)
	{
		while(EPROM_Read(0x0013, 0x50, 10,Rcv) == NOT_OK);
		while(EPROM_Read(0x0033, 0x50, 10,Rcv) == NOT_OK);
	}
} 
