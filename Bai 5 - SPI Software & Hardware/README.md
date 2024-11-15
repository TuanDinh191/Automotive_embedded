
# Bài 5: SPI software & Hardware
#### 1. SPI Software
 - Để sử dùng SPI software thì đầu tiên ta phải xác định các chân GPIO để giao tiếp SPI sau đó đến bước cấp clock cho GPIO đến bước cấu hình GPIO rồi đến bước sử dụng GPIO để giao tiếp SPI, các ngoại vi khác liên quan đến SPI thì làm điều tương tự.
- Trước khi sử dụng ngoại vi ta sẽ cấp clock trước
```
 void RCC_Config(){
 RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, 
ENABLE);
 }
 ```
 - Ở master ta sẽ cấu hình chế độ là ngõ ra push pull đối với những chân SCK, MOSI, CS. Riêng với chân MISO là chân master sẽ nhận dữ liệu từ slave thì sẽ cấu hình là chế độ là ngõ vào floating.

 ```
 void GPIO_Config(){
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = SPI_MISO_Pin; 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
 }
 ```

- Đối với Slave ta sẽ cấu hình chân SCK MOSI CS là chế độ là ngõ vào floating bởi vì các chân này đều nhận dữ liệu bên master và do master điều khiển. Còn chân MISO thì slave sẽ nắm quyền điều khiển nên sẽ cấu hình là chế độ ngõ ra push pull.

```
vvoid GPIO_Config(){
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = SPI_MISO_Pin;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
 }
 ```
- Ở master để tạo xung SCK thì ta sẽ set bit là cao hoặc thấp đối với chân SCK tùy vào chế độ nghỉ của xung SCK. Ở đây ta sẽ set chế độ nghỉ của xung SCK là lúc xuống CPOL = 0. 
```
 void Clock(){
 GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_SET);
 delay_ms(4);
 GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
 delay_ms(4);
 }
```
- Chế độ ban đầu master trước khi bắt đầu truyền hoặc nhận dữ liệu là chân CS sẽ được kéo lên và chân MISO và MOSI sẽ là kéo xuống.  
```
void SPI_Init(){
 GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
 GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
 GPIO_WriteBit(SPI_GPIO, SPI_MISO_Pin, Bit_RESET);
 GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
 }
 ```
- Hàm truyền trong master sẽ truyền dữ liệu 8 lần (1 bit 1 lần) tương ứng với 1 byte và sẽ kèm theo xung clock, trước khi truyền dữ liệu thì master sẽ kéo chân CS xuống mức 0 để xác định chọn slave nào để giao tiếp sau khi master truyền xong thì sẽ kéo chân CS lên là 1 để báo quá trình truyền dữ liệu đã hoàn thành.  

```
void SPI_Master_Transmit(uint8_t u8Data){ //0b10010000
 uint8_t u8Mask = 0x80; // 0b10000000
 uint8_t tempData;
 GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_RESET);
 delay_ms(1);
 for(int i = 0; i < 8; i++){
 tempData = u8Data & u8Mask;
 if(tempData){
 GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_SET);
 delay_ms(1);
 } else{
 GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
 delay_ms(1);
 }
 u8Data = u8Data << 1;
 Clock();
 }
 GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
 delay_ms(1);
 }
 ```

 - Hàm nhận trong slave sẽ nhận tương ứng 8 lần (1 lần 1 bit) từ master, trước khi nhận từ master thì slave sẽ kiểm tra chân CS có kéo xuống 0 để kiểm tra master có muốn giao tiếp với slave này không và kiểm tra chân SCK có kéo lên không để nhận dữ liệu, khi xung clock kéo xuống thì slave sẽ không làm gì, sau khi nhận đủ dữ liệu thì slave sẽ kiểm tra chân CS có kéo lên là 1 chưa để biết tiếp tục hay dừng quá trình nhận dữ liệu từ  master.    


 ```
uint8_t SPI_Slave_Receive(void){
 uint8_t dataReceive = 0x00; //0b0000 0000
 uint8_t temp = 0x00;
 while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
 while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
 for(int i = 0; i < 8; i++){ 
if(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
 while (GPIO_ReadInputDataBit(SPI_GPIO, 
SPI_SCK_Pin)){
 temp = GPIO_ReadInputDataBit(SPI_GPIO, 
SPI_MOSI_Pin);
 }
 dataReceive <<= 1;
 dataReceive |= temp;
    }
 while(!GPIO_ReadInputDataBit(SPI_GPIO, 
SPI_SCK_Pin));
 }
 while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
 return dataReceive;
 }
 
 ```
 - Hàm main trong master sẽ gọi hàm truyền 7 lần tương ứng với truyền 77 byte dữ liệu trước khi gọi hàm truyền thì sẽ gọi hàm cấp clock và cấu hình ngoại vi liên quan.

 ```
  uint8_t DataTrans[] = {1,3,9,10,15,19,90};//Data
 int main(){
 RCC_Config();
 GPIO_Config();
 TIM_Config();
 SPI_Init();
 while(1){
 for(int i = 0; i < 7; i++){
 SPI_Master_Transmit(DataTrans[i]);
 delay_ms(1000);
 }
 }
 }
 ```
 - Hàm main bên slave sẽ nhận dữ liệu từ master.
 ```
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
```	
	
#### 2. SPI Hardware
- Để dùng SPI Hardware thì ta sẽ xác định chân nào nó có chức năng Alternate Function hỗ trợ giao tiếp SPI bằng cách tra pin definition trong datasheet của dòng vi điểu khiển đó.
- Cấu hình chân bên master:
```
 void GPIO_Config(){
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = SPI1_NSS | SPI1_SCK | SPI1_MISO | SPI1_MOSI;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 }
 ```
- Cấu hình chân bên slave:
``` 
 void GPIO_Config(){
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = SPI1_NSS | SPI1_SCK | SPI1_MISO | SPI1_MOSI;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 }
 GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
 ```
 - Cấu hình SPI sẽ qua các thành phần trong struct SPI_InitTypeDef:

SPI_mode : cấu hình chế độ thiết bị là master hoặc là slave.

SPI_mode : cấu hình chế độ truyền đơn công, bán song công hoặc song công.

SPI_BaudratePrescaler: Hệ số chia clock cấp cho SPI.

SPI_CPOL: CPOL_LOW cấu hình xung clock chế độ nghỉ là mức 0, CPOL_HIGH cấu hình xung clock chế độ nghỉ là mức 1.

SPI_CPHA: CPHA_LOW đọc dữ liệu tại cạnh đầu tiên của xung clock và chuyển đổi dữ liệu tại cạnh thứ 2 của xung clock, CPHA_HIGH sẽ chuyển đổi dữ liệu tại cạnh đầu tiên của xung clock và đọc dữ liệu tại cạnh thứ 2 của xung clock.

SPI_DataSize: Cấu hình số bit truyền. 8 hoặc 16 bit.

SPI_FirstBit: Cấu hình chiều truyền của các bit là MSB hay LSB.

SPI_CRCPolynomial: Cấu hình số bit CheckSum cho SPI.

SPI_NSS: Cấu hình chân SS là điều khiển bằng thiết bị hay phần mềm.

- Để truyền 1 byte data trong SPI thì ta sẽ gọi hàm SPI_Send1Byte dưới đây

Dòng đầu tiên ta sẽ kéo chân CS về 0 để master chọn slave giao tiếp sau đó sẽ gọi hàm kiểm cờ TXE (không có dữ liệu trong buffer truyền) không nếu có thì sẽ thực hiện hàm truyền `SPI_I2S_SendData(SPI1, data)` tham số truyền vào của hàm này là tham số thứ nhất sẽ chọn bộ SPI để truyền , tham số thứ 2 là data truyền là 8 bit hoặc 16 bit. Sau đó gọi hàm kiểm tra cờ BSY coi ngoại vi SPI có bận đang truyền hay nhận không, nếu đã hoàn thành xong rồi thì sẽ kéo chân CS lên là 1.

```
 void SPI_Send1Byte(uint8_t data){
 GPIO_ResetBits(GPIOA, SPI1_NSS);
 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
 SPI_I2S_SendData(SPI1, data);
 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
 GPIO_SetBits(GPIOA, SPI1_NSS);
 }
 ```
- Để nhận 1 byte data trong SPI thì ta sẽ gọi hàm SPI_Receive1Byte dưới đây

Dòng đầu tiên sẽ kiểm tra ngoại vi SPI bên slave có bận đang nhận dữ liệu không nếu không thì sẽ gọi hàm `SPI_I2S_ReceiveData` để lấy dữ liệu từ master truyền đến sau khi đã lấy dữ liệu rồi sẽ kiểm tra cờ RXNE (có dữ liệu trong buffer nhận) sau khi lấy thì dữ liệu trong buffer đã trống chưa rồi sau đó mới trả về giá trị. 
```
  uint8_t SPI_Receive1Byte(void){
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
    uint8_t temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return temp;
 }
 ```
- Hàm main trong master sẽ gọi hàm truyền 7 lần tương ứng với truyền 77 byte dữ liệu trước khi gọi hàm truyền thì sẽ gọi hàm cấp clock và cấu hình ngoại vi liên quan.
```
 uint8_t dataSend[] = {3, 1, 10, 19, 20, 36, 90};
 int main(){
 RCC_Config();
 GPIO_Config();
 TIM_Config();
 SPI_Config();
 while(1){
 for(int i = 0; i < 7; i++){
 SPI_Send1Byte(dataSend[i]);
 delay_ms(1000);
 }
 }
 }
 ```
- Hàm main bên slave sẽ nhận dữ liệu từ master sẽ kiểm tra chân CS đã kéo xuống là 0 chưa để xác định master chọn slave này để giao tiếp sau đó vào hàm nhận để nhận dữ liệu từ master.

```
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