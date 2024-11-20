
# Bài 6: I2C Software & I2C Hardware
#### 1. I2C Software
 - Để sử dùng I2C software thì đầu tiên ta phải xác định các chân GPIO để giao tiếp I2C sau đó đến bước cấp clock cho GPIO đến bước cấu hình GPIO rồi đến bước sử dụng GPIO để giao tiếp I2C, các ngoại vi khác liên quan đến I2C thì làm điều tương tự.
- Trước khi sử dụng ngoại vi ta sẽ cấp clock trước
```
#define I2C_SCL	  GPIO_Pin_1
#define I2C_SDA   GPIO_Pin_2

#define I2C_GPIO  GPIOA


void RCC_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
 ```
 - Ở master ta sẽ cấu hình chế độ là ngõ ra opendrain của chân SCL SDA.

 ```
void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO,&GPIO_InitStructure);
}
 ```

- Giao tiếp I2C thường được sử dụng khi có master là một MCU còn slave là thiết bị phần cứng chẳng hạn mạch thời gian Tiny RTC I2C 32 kbit, LCD 1602...

- Ở master để tạo xung SCL 100 Khz theo xung clock datasheet của slave thì ta sẽ tạo thời gian xung kéo lên là 5 us và thời gian xung kéo xuống là 5 us.

```
#define WRITE_SDA_0 GPIO_ResetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SDA_1 GPIO_SetBits(I2C_GPIO,I2C_SDA)
#define WRITE_SCL_0 GPIO_ResetBits(I2C_GPIO,I2C_SCL)
#define WRITE_SCL_1 GPIO_SetBits(I2C_GPIO,I2C_SCL)
#define READ_SDA_VAL GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA)

//SCL clock
WRITE_SCL_1;
delay_us(5);
WRITE_SCL_0;
delay_us(5);
```
- Chế độ nghỉ ban đầu master trước khi bắt đầu truyền hoặc nhận dữ liệu là chân SDA và chân SCL sẽ được kéo lên. 
```
void I2C_Config()
{
	WRITE_SDA_1;
	delay_us(1);
	WRITE_SCL_1;
	delay_us(1);
}
 ```
- Để bắt đầu quá trình truyền và nhận dữ liệu trong I2C thì ta sẽ tạo ra Start_condition là điểu kiện bắt đầu. Ta sẽ kéo chân SDA từ mức cao về mức thấp trước khi kéo chân SCL 

```
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
```

- Hàm truyền sẽ truyền dữ liệu 8 lần (1 bit 1 lần) tương ứng với 1 byte và sẽ kèm theo xung clock, sau khi truyền dữ liệu xong thì master sẽ kéo chân SDA lên là 1 để đợi tín hiệu ACK của slave để nhận phản hồi từ slave và đọc dữ liệu tại thời điểm giữa xung kéo lên là nơi có dữ liệu ổn định nhất.

```
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
 ```

 - Hàm nhận sẽ nhận tương ứng 8 lần (1 lần 1 bit) từ master. Sau khi master nhận đủ 8 bit dữ liệu thì nếu master gửi lại ACK cho slave thì thông báo cho slave tiếp tục truyền dữ liệu tiếp theo hoặc nếu là NACK sẽ thông báo cho slave là không nhận dữ liệu tiếp theo nữa, hàm sẽ trả về giá trị dữ liệu nhận được.      

 ```
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
 ```

- Sau quá trình truyền và nhận dữ liệu thì ta sẽ ta sẽ tạo ra Stop condition là để xác nhận quá trình truyền nhận dữ liệu đã hoàn thành. Ta sẽ kéo chân SCL lên mức 1 trước khi kéo chân lên mức 1 đối với chân SDA. 

```
void I2C_Stop()
{
	WRITE_SDA_0;
	delay_us(3);
	WRITE_SCL_1;
	delay_us(3);
	WRITE_SDA_1;
	delay_us(3);
}
```

- Ở hàm main ta sẽ dùng hai hàm EPROM_Write và hàm EPROM_Read để ghi và đọc từ slave một thiết bị phần cứng cụ thể đọc và ghi dữ liệu EEPROM của mạch thời gian HW-11 hai hàm này ta sẽ viết theo khung dữ liệu truyền và đọc dữ liệu theo Byte Write và Random Read theo datasheet của EEPROM AT24C32/64. 

```
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

```
	
#### 2. I2C Hardware

- Để dùng I2C Hardware thì ta sẽ xác định chân nào nó có chức năng Alternate Function hỗ trợ giao tiếp I2C bằng cách tra pin definition trong datasheet của dòng vi điểu khiển đó.

```

#define I2C_SCL 	GPIO_Pin_6
#define I2C_SDA 	GPIO_Pin_7

#define I2C_GPIO 	GPIOB

void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO,&GPIO_InitStructure);
}
 ```

 - Cấu hình I2C sẽ qua các thành phần trong struct I2C_InitTypeDef:

I2C_mode : cấu hình chế độ thiết bị là hoạt động chế độ fast mode hoặc là low mode.

I2C_ClockSpeed : cấu hình clock đối với chế độ slow mode có thể set giá trị clock tới 100khz và set giá trị clock tới 400khz ở chế độ fastmode.

I2C_DutyCycle: cấu hình chu kỳ xung là thời gian xung thấp sẽ bằng 2 lần xung cao hoặc thời gian xung thấp sẽ bằng 16/9 thời gian xung cao.  

I2C_OwnAddress1: Cấu hình địa chỉ thiết bị đang cấu hình.

I2C_ACK: Cấu hình ACK, master có sử dụng ACK hay không.

I2C_AcknowledgedAddress: Cấu hình số bit địa chỉ 7 hoặc 10 bit.

```
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
```
- Hàm truyền nhận trong I2C:

`I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState)` hàm này có chức năng tạo điều kiện bắt đầu để bắt đầu quá trình truyền hoặc nhận dữ liệu, tham số thứ nhất truyền vào là chọn bộ I2C để bắt đầu điều kiện, tham số thứ 2 là có tạo điều kiện hoặc không. 

` I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState)` hàm này có chức năng tạo điều kiện kết thúc sau khi đã hoàn thành quá trình truyền và nhận dữ liệu, tham số thứ nhất truyền vào là chọn bộ I2C để bắt đầu điều kiện, tham số thứ 2 là có tạo điều kiện hoặc không. 

`I2C_Send7BitAddress(I2C_TypeDef *I2C, uint8_t Address, uint8_t I2C_Direction)` hàm này có chức năng sẽ gửi đi 7 bit địa chỉ để xác nhận giao tiếp, tham số truyền vào của hàm là tham số thứ nhất là chọn bộ I2C để giao tiếp, tham số thứ 2 là địa chỉ của thiết bị để giao tiếp, tham số thứ 3 là chọn kiểu truyền hoặc là nhận dữ liệu là bit thứ 8 Read/Write set `I2C_Direction_Transmitter` để truyền hoặc `I2C_Direction_Receiver` để nhận.

`I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data)` hàm này có chức năng gửi đi 8 bit data tham số thứ nhất truyền vào là chọn bộ I2C, tham số thứ 2 là chọn Data để truyền. 

` I2C_ReceiveData(I2C_TypeDef* I2Cx)` hàm này sẽ có chức năng trả về 8 bit data nhận được, tham số thứ nhất truyền vào là chọn bộ I2C để nhận dữ liệu. 

`I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)` hàm này có chức năng kích hoạt master có gửi ACK hay không nếu không thì master sẽ gửi NACK để xác nhận là không nhận dữ liệu nữa, tham số thứ nhất truyền vào là chọn bộ I2C, tham số thứ 2 là có kích hoạt hoặc không. 

`I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)` hàm này có chức năng sẽ kiểm tra các sự kiện xảy ra trong giao tiếp I2C, tham số thứ nhất truyền vào là chọn bộ I2C để kiểm tra sự kiện, tham số thứ 2 sẽ là cờ/sự kiện mà hàm này sẽ kiểm tra. 

- Các cờ/sự kiện sử dụng thông dụng xảy ra trong I2C:

`I2C_EVENT_MASTER_MODE_SELECT` Đợi Bus I2C về chế độ rảnh.

`I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED` Đợi xác nhận đã gửi 7 bit địa chỉ thiết bị để giao tiếp kèm với 1 bit Write và đã nhận được ACK phản hồi từ thiết bị giao tiếp để sẵn sàng quá trình truyền dữ liệu. 

`I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ` Đợi xác nhận đã gửi 7 bit địa chỉ thiết bị để giao tiếp kèm với 1 bit Read và đã nhận được ACK phản hồi từ thiết bị giao tiếp để sẵn sàng quá trình nhận dữ liệu. 

`I2C_EVENT_MASTER_BYTE_TRANSMITTED` Đợi truyền xong 1 byte data từ Master.

`I2C_EVENT_MASTER_BYTE_RECEIVED` Đợi Master nhận đủ 1 byte data.

- Các ví dụ khi sử dụng hàm và các cờ sự kiện:

```
I2C_GenerateSTART(I2C1, ENABLE);
 //Waiting for flag
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
 I2C_Send7bitAddress(I2C1, 0x44, I2C_Direction_Transmitter);
 //And check the transmitting
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 ```

 ```
  void Send_I2C_Data(uint8_t data)
 {
 	I2C_SendData(I2C1, data);
 	// wait for the data trasnmitted flag
 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	 
```

```
 uint8_t Read_I2C_Data(){
 uint8_t data = I2C_ReceiveData(I2C1);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
 return data;
 }
```
