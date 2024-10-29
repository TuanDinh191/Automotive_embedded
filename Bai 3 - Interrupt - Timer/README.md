
# Bài 3: INTERRUPT - TIMER
#### 1. Ngắt (Interrupt) trong vi điều khiển 
#### - Định nghĩa: 
- Ngắt là 1 sự kiện khẩn cấp xảy ra trong hoặc ngoài vi điều khiển. Khi sự kiện ngắt xảy ra thì nó yêu cầu MCU dừng chương trình chính để thực thi chương trình ngắt.
- Mỗi ngắt thì sẽ có cờ ngắt, vector ngắt và độ ưu tiên ngắt. Khi cờ ngắt được set giá trị lên là 1 thì sẽ báo MCU dừng lại và gọi vector ngắt (địa chỉ trong bộ nhớ của ISR) và thực thi trình phục vụ ngắt (ISR) là một chương trình thực thi riêng khi có ngắt xảy ra. Độ ưu tiên được dùng khi có từ 2 sự kiện ngắt xảy ra cùng một lúc, cái ngắt nào có độ ưu tiên cao hơn thì sẽ thực thi trước, độ ưu tiên ngắt thì mình có thể lập trình được. 
#### - Cơ chế ngắt xảy ra trong MCU:
- Khi đang thực thi trong một chương trình thì sẽ có PC (Program Counter) là thanh ghi luôn trỏ đến địa chỉ của lệnh tiếp theo trong chương trình thì khi chương trình đã thực thi xong dòng lệnh hiện tại thì chương trình sẽ theo PC để thực thi lệnh tiếp theo trong chương trình. Khi có ngắt xảy ra thì PC sẽ trỏ tới vector ngắt của ISR đó, trước khi PC trỏ tới vector ngắt thì PC sẽ lưu địa chỉ hiện tại của chương trình chính vào Main Stack Pointer (MSP), trong lúc chương trình đang thực thi ISR thì PC đã quay lại chương trình chính lấy lại giá trị đã lưu vào MSP, sau khi chương trình thực thi xong ISR sẽ quay trở lại chương trình chính tiếp tục thực hiện lệnh còn lại. Trong trường hợp khi chương trình đang thực hiện ISR1 ngay lập tức có ngắt của ISR2 sẽ có độ ưu tiên cao hơn lúc đó PC sẽ lưu ISR1 vào MSP sau đó trỏ vào vector ngắt của ISR2.  
#### 2. Các ngắt thông dụng
Có 3 loại ngắt thông dụng được dùng nhiều nhất là ngắt ngoài, ngắt timer và ngắt truyền thông: 
- Ngắt ngoài: Khi các chân GPIO được cấu hình chế độ là ngõ ngắt và có sự thay đổi về điện áp trên các chân này. Có 4 dạng:   
LOW: Kích hoạt ngắt liên tục khi chân ở mức thấp.

HIGH: Kích hoạt ngắt liên tục khi chân ở mức cao.

RISING: Kích hoạt khi trạng thái trên chân chuyển từ thấp lên cao.

FALLING: Kích hoạt khi trạng thái trên chân chuyển từ cao xuống thấp.
- Ngắt timer: Khi trong thanh ghi đếm của timer bị tràn thì sẽ thực thi một cái ISR. Để tạo ra lần ngắt tiếp theo thì phải reset giá trị trong thanh ghi.
- Ngắt truyền thông: Khi có sự kiện truyền nhận dữ liệu giữa MCU và các thiết bị khác, thường sử dụng cho các giao thức UART, SPI, I2C để đảm bảo việc truyền nhận được chính xác. Cho một ví dụ để hiểu rõ hơn: Khi có MCU A truyền dữ liệu cho MCU B bằng UART thì 2 con này đều lập trình canh thời gian bằng nhau để  khi MCU A sẽ truyền thì MCU B sẽ nhận dữ liệu. Nhưng thực tế thì sẽ có sự chênh lệch làm cho MCU A khi thực thi hàm truyền dữ liệu thì trong lúc đó thì MCU B vẫn chưa thực thi đến hàm nhận dữ liệu thì sẽ mất dữ liệu nhận đi để tránh việc này thì sẽ dùng sự kiện nhận dữ liệu làm sự kiện ngắt khi MCU B có tín hiệu dữ liệu nhận được từ MCU A thì ngay lập tức chạy ISR có hàm nhận dữ liệu.
#### 2. Timer
#### - Định nghĩa:
-  Timer là 1 mạch digital logic có vai trò đếm mỗi chu kỳ clock (đếm lên hoặc đếm xuống).
- Timer còn có thể hoạt động ở chế độ nhận xung clock từ các tín hiệu ngoài. Ngoài ra còn các chế độ khác như PWM, định thời …vv.
#### - Cấu hình timer:
- Để cấu hình timer thì khởi tạo biến có kiểu dữ liệu là TIM_TimeBaseInitTypeDef; 
```
typedef struct
{
  uint16_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */ 

  uint16_t TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */

  uint8_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the RCR value (N).
                                       This means in PWM mode that (N+1) corresponds to:
                                          - the number of PWM periods in edge-aligned mode
                                          - the number of half PWM period in center-aligned mode
                                       This parameter must be a number between 0x00 and 0xFF. 
                                       @note This parameter is valid only for TIM1 and TIM8. */
} TIM_TimeBaseInitTypeDef; 

```
Giải thích trong TIM_TimeBaseInitTypeDef:

- TIM_ClockDivision: chọn bộ chia tần số.  
- TIM_Prescaler: bao nhiêu dao động thì timer sẽ đếm lên 1 lần.
 - TIM_Period: Timer sẽ đếm bao nhiêu lần thì nó sẽ bị tràn.
 - TIM_CounterMode: Chọn 1 trong 2 chế độ, đếm lên hay đếm xuống.
#### - Sử dụng Timer:

Ta sẽ dùng các hàm này khi dùng Timer:

` void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter) `
- Hàm này có tác dụng đặt giá trị ban đầu cho timer bắt đầu đếm.
`uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);`
- Hàm này có tác dụng lấy giá trị hiện tại khi timer đang đếm.

