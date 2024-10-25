
# Bài 1: Setup project đầu tiên trên Keil C
#### 1. Tải phần mềm và thư viện Keil C trên máy tính
- Để dùng phần mềm KeilC thì trước tiên mình phải tải phần mềm KeilC Uvision5 về máy tính.
- Tùy thuộc vào phần cứng mình đang dùng con vi điều khiển STM32 dòng nào thì phải tải thư viện chuẩn dòng đó trên trang chủ của keil.
Link tải thư viện : https://www.keil.arm.com/devices/
- Mục đích tải thư viện thì khi code mình sẽ dùng các API và các hàm có sẵn của 1 dòng vi điều khiển sẽ tiết kiệm thời gian code mà code sau này sẽ dễ bảo trì hơn.
#### 2. Tạo 1 project trên phần mềm KeilC
- Mở phần mềm KeilC chọn Project --> click vào New uVision Project--> chọn thư mục mình đã tạo sẵn -- > đặt tên project.
- Lúc đó sẽ hiện bảng "Select Device for Target 1" --> click vào chọn dòng điều khiển STM32 mình đang dùng.
- Sau đó sẽ hiện bảng "Manage-RunTime Environment" --> Click vào Startup để chứa các setup để khởi động vi điều khiển, để KeilC nhận diện mình đang dùng con vi điều khiển nào --> Click vào resolve.
- Trong cửa sổ project click phải source group 1 chọn "Add New Item..." --> Chọn định dạng .c sau đó đặt tên file là main.c.
- Sau đó vào Options for Target thiết lập thêm cài đặt: tại mục Target chọn ARM compiler là "Use default compiler version 5" và tại mục C/C++ ta cũng phải click vào C99 Mode, và cũng tại mục debug ta chọn ST-link Debugger để chọn phần cứng để debug.
#### 3- Tạo 1 project BlinkLed trên PD14 của STM32F4
#### - Cấp xung clock cho pin PD14 của GPIOD
- Để một chân pin GPIOD pin PD14 hoạt động thì trước tiên ta phải cấp xung clock thông qua bus AHB1 từ xung clock nội HSI của vi xử lí.
- Để cấu hình xung clock cho ngoại vi GPIO thì ta phải vào file pdf Reference Manual của STM32F407 trên mục Books tại phần project của KeilC.
- Tại mục "6.3.10 RCC AHB1 peripheral clock register (RCC_AHB1ENR)" của Reference manual để enable clock cho GPIOD  thì ta phải set bit tại vị trí thứ 3 trong thanh ghi 0x30 lên là bit là 1.
- Để cấu hình xung clock cấp cho GPIOD thì ta dùng lệnh:
```bash
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
```
- RCC này đã được định nghĩa dưới dạng là một struct nếu muốn dùng thì chỉ cần trỏ đến AHB1ENR và RCC_AHB1ENR_GPIODEN được định nghĩa set bit 1 tại vị trí thứ 3.
- Việc dùng phép "|=" ưu tiên hơn là phép "=" bởi vì trong ghi RCC đã set bit 1 cấp xung clock hoạt động cho các ngoại vi khác nữa ngoài GPIOD. Khi dùng phép "|=" thì khi cấp clock cho GPOID thì sẽ không ảnh hưởng đến cấp clock các ngoại vi khác thuộc bus AHB1.
#### - Cấu hình chế độ chân GPIOD
- Cấu hình tốc độ chân PD14 với tốc độ là highspeed theo mục 8.4.3 GPIO port output speed register (GPIOx_OSPEEDR)(x = A..I/J/K)
```
GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1;
```
- Cấu hình chân PD14 là có chế độ là push-pull theo mục 8.4.6 GPIO port output data register (GPIOx_ODR) (x = A..I/J/K).
```
GPIOD->OTYPER |= (~GPIO_OTYPER_ODR_14);
```
- Cấu hình chân PD14 có chế độ là ngõ ra output theo mục 8.4.1 GPIO port mode register (GPIOx_MODER) (x = A..I/J/K
```bash
GPIOD->MODER &=  ~(GPIO_MODER_MODER14_0);
GPIOD->MODER |=   GPIO_MODER_MODER14_0;
```
- Ở đây ta dùng phép "&= ~" để reset chế độ chân PD14 tại bit thứ 28 và bit thứ 29 về là 0 không set chế độ nào nữa rồi mới set chế độ chân PD14 là ngõ ra.
#### - Ghi tín hiệu ra chân PD14
- Thực hiện chớp tắt led PD14 trong một dòng lặp while, cấu hình xuất dữ liệu chân PD14 theo mục 8.4.6 GPIO port output data register (GPIOx_ODR) (x = A..I/J/K). 
- Bit thứ 14 set 1 là bật 0 là tắt.
```bash
	while (1)
	{
		GPIOD->ODR |= 1<<14;
		delay(300000);
		GPIOD->ODR &= ~(1<<14);
		delay(300000);
	}
```
#### 4- Nhấn nút pin PA0 để toggle LED PD13
- Cấu hình thêm tại pin GPIOA PA0 là input là một nút nhấn, lý do không dùng điện trở kéo lên hay kéo xuống tại dòng `GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0` theo schematic của stm32f407 thì nút nhấn PA0 đã được mắc với điện trở kéo lên.
- Khi nhấn nút thì sẽ qua hàm toggle làm led sáng lên nhấn nút thêm 1 lần nữa thì sẽ tắt đi cứ vậy theo vòng lặp while lặp lại.   
```
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
```



