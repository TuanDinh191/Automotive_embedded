
# Bài 2: GPIO
#### 1. Thư viện Standard peripheral Firmware Library
- Là 1 thư viện phát triển bởi ST cho dòng STM32, đây là 1 thư viện hoàn chỉnh cung cấp đủ driver (các hàm, API) cho các tất cả các ngoại vi tiêu chuẩn mà tùy dòng vi điều khiển trang bị. 
- Ở thư viện này mình sẽ dùng các hàm, cấu trúc dữ liệu và macro mà nó cấu hình ngoại vi.
- Ở bài này chúng ta sẽ dùng driver rcc và gpio nên sẽ include vào file.
```
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
```
#### 2. Cấu hình ngoại vi GPIO pin PD14
-  Trước khi cấu hình ngoại vi ta sẽ bắt đầu cấp clock cho ngoại vi, sau khi cấu hình ngoại vi xong ta sẽ sử dụng ngoại vi ở đây ta sẽ dùng thư viện SPL.
-  Theo sơ đồ của "STM32F40xxx block diagram" thì để cấp xung cho PD14 thì ta sẽ cấp xung qua AHB1 bus nên ta sẽ dùng hàm `RCC_AHB1PeriphClockCmd`
- Nhấn chuột phải hàm thì nó sẽ hiện Go to definition of `RCC_AHB1PeriphClockCmd` nhấn vào đó hoặc có thể nhấn phím F12 thì sẽ hiện một cửa sổ ghi chú tại đây sẽ chú thích những tham số truyền vào hàm.
```
/**
  * @brief  Enables or disables the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:       GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:       GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:       GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:       GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:       GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:       GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:       GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:       GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:       GPIOK clock (STM32F42xxx/43xxx devices)  
  *            @arg RCC_AHB1Periph_CRC:         CRC clock
  *            @arg RCC_AHB1Periph_BKPSRAM:     BKPSRAM interface clock
  *            @arg RCC_AHB1Periph_CCMDATARAMEN CCM data RAM interface clock
  *            @arg RCC_AHB1Periph_DMA1:        DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:        DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:       DMA2D clock (STM32F429xx/439xx devices)  
  *            @arg RCC_AHB1Periph_ETH_MAC:     Ethernet MAC clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Tx:  Ethernet Transmission clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Rx:  Ethernet Reception clock
  *            @arg RCC_AHB1Periph_ETH_MAC_PTP: Ethernet PTP clock
  *            @arg RCC_AHB1Periph_OTG_HS:      USB OTG HS clock
  *            @arg RCC_AHB1Periph_OTG_HS_ULPI: USB OTG HS ULPI clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
  ```
- `RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE)` ở hàm này để cấp clock cho pin PD14 thì ta phải truyền vào tham số là `enable` và ngoại vi cần enable là GPIOD nên tham số truyền vào là `RCC_AHB1Periph_GPIOD`
- Để cấu hình thêm cho chân PD14 thì ta phải khởi tạo một cái struct có kiểu dữ liệu là `GPIO_InitTypeDef `.
```
typedef struct
{
  uint32_t GPIO_Pin;              /*!< Specifies the GPIO pins to be configured.
                                       This parameter can be any value of @ref GPIO_pins_define */

  GPIOMode_TypeDef GPIO_Mode;     /*!< Specifies the operating mode for the selected pins.
                                       This parameter can be a value of @ref GPIOMode_TypeDef */

  GPIOSpeed_TypeDef GPIO_Speed;   /*!< Specifies the speed for the selected pins.
                                       This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOOType_TypeDef GPIO_OType;   /*!< Specifies the operating output type for the selected pins.
                                       This parameter can be a value of @ref GPIOOType_TypeDef */

  GPIOPuPd_TypeDef GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                                       This parameter can be a value of @ref GPIOPuPd_TypeDef */
}GPIO_InitTypeDef;
```
- Trong struct này có chứa các thành phần là các enum để lựa chọn chế độ cho từng thành phần thì ta có thể Ctrl F để nhập từ khóa mà chú thích ghi từng enum để biết giá trị truyền vào.
- `GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;` dòng này ta sẽ chọn chân 14 là chân ta cần thao tác theo GPIO_pins_define được định nghĩa.
```
#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */
```
- Tương tự đối với 

 	`GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;`

	 `GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;`

- Chọn chế độ chân PD14 là chân ngõ ra và có tốc độ là 100Mhz.
- Sau khi đã set chế độ cho ngoại vi cho chân PD14 thì ta cần gọi hàm `GPIO_Init(GPIOD,&GPIO_InitStruct);`để gán giá trị mà ta đã set vào thanh ghi.
#### 3. Sử dụng ngoại vi
Để sử dụng giá trị nhận hay vào hay xuất ra cho cho port GPIO thì ta có thể dùng các hàm như sau:

	   `void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)`

- Hàm này có chức năng set giá trị ngõ ra cho một port GPIO, tham số thứ nhất truyền vào là port GPIO nào, tham số thứ 2 truyền vào là giá trị cho 16 chân ngõ ra.
  

	`void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`

- Hàm này có chức năng set bit lên giá trị là 1 cho một chân của một port GPIO, tham số thứ nhất truyền vào là port GPIO nào, tham số thứ 2 truyền vào là cụ thể pin nào của port GPIO.

  `void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`

- Ngược lại với hàm `void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`
	 
	`uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`

- Hàm này có chức năng đọc giá trị đầu vào đối với một chân của một port GPIO và có giá trị trả về 1 hoặc là 0.

	`uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`

- Hàm này có chức năng đọc giá trị đầu vào đối với một chân ngõ ra của một port GPIO và có giá trị trả về là 1 hoặc 0.
