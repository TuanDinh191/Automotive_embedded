
# Bài 4: Communication Protocols
#### 1. Đặt vấn đề:
 - Khi muốn truyền nhiều dữ liệu từ MCU A đến MCU B thì nó sẽ truyền dữ liệu qua là các bit 1 và 0 cụ thể hơn là các tín hiệu điện áp giữa các chân pin của MCU (bit 1 cho điện áp là 3.3V, bit 0 là cho 0V). Nhưng khi truyền liên tiếp nhiều dữ liệu từ MCU A sang MCU B thì làm sao MCU B phân biệt từng bit riêng lẻ ví dụ cụ thể là khi MCU A truyền liên tiếp 3 bit giống nhau là 111 hoặc 000 thì bằng cách nào mà MCU B phân biệt bit này cụ thể tại thời điểm nào ở trước hay là sau. Để giải quyết vấn đề khi có tập dữ liệu bit giống nhau liền kề thì ta sẽ sinh ra các chuẩn giao tiếp để phân biệt được từng bit khi truyền nhận dữ liệu. 
#### 2. SPI:
#### - Định nghĩa: 
- Giao thức SPI (Serial Peripheral Interface): đây là một chuẩn giao tiếp nối tiếp, có thể giao tiếp đồng bộ ( ví dụ giữa 2 thiết bị cụ thể hơn khi có 2 thiết bị giao tiếp với nhau thì sẽ thống nhất với nhau tại 1 thời điểm nhận hoặc truyền dữ liệu) có thể hoạt động ở 3 chế độ là đơn công (master chỉ truyền cho slave hoặc chỉ có slave truyền cho master, một thiết bị chỉ làm 1 chức năng nhận hoặc truyền  ), bán song công (master có thể truyền cho slave hoặc master nhận từ slave nhưng không cùng 1 thời điểm) song công (Có thể truyền nhận cùng thời điểm).
SPI sử dụng 4 dây để giao tiếp:
- SCK (Serial Clock): Chân tín hiệu xung clock được tạo bởi Master truyền đến cho slave.
- MISO (Master Input Slave Output): Chân dữ liệu được tạo bởi Slave truyền về Master.
- MOSI (Master Output Slave Input): Chân dữ liệu được tạo bởi Master truyền về Slave.
- SS (Slave Select/Chip Select): Chân để chọn slave cụ thể giao tiếp thì master sẽ kéo chân SS xuống mức 0 (0v).
#### - Quá trình truyền SPI:
- Để bắt đầu quá trình truyền thì master sẽ kéo chân CS của slave muốn giao tiếp xuống mức 0V để bắt đầu quá trình truyền nhận.
- Master truyền dữ liệu Slave nhận: Khi 1 bit truyền đi tại chân MOSI của master thì sẽ kèm theo 1 xung clock tại chân SCK thì slave đợi khi nào đọc được xung clock tại SCK có giá trị xung là cạnh lên thì sẽ đọc dữ liệu tại chân MOSI và lưu giá trị dữ liệu lại.
- Master nhận dữ liệu từ Slave: Khi master truyền 1 xung clock thì sẽ canh khi nào SCK có giá trị xung là cạnh lên thì sẽ lập tức đọc dữ liệu từ chân MISO trong lúc đó khi slave đã đọc được xung clock gửi từ master có giá trị là cạnh lên thì lập tức slave sẽ truyền 1 bit dữ liệu về cho master.
- Trong quá trình đọc dữ liệu của master hoặc slave hoặc cả master và slave cùng một lúc: Khi đã nhận dữ liệu rồi thì cập nhật giá trị trong các thanh ghi và dịch 1 bit.
- Lập lại quá trình trên cho đến khi truyền xong 8 bit trong thanh ghi.
#### - Chế độ hoạt động của SPI:
- CPOL = 0 : Khi cấu hình `cpol = 0 ` thì xung clock có trạng thái rảnh là bằng 0.
- CPOL = 1 : Khi cấu hình `cpol = 1 ` thì xung clock có trạng thái rảnh là bằng 1.
- CPHA = 0 : Khi cấu hình `cpha = 0 ` thì sẽ đọc dữ liệu tại cạnh đầu tiên của xung clock và chuyển đổi dữ liệu tại cạnh thứ 2 của xung clock. 
- CPHA = 1 : Khi cấu hình `cpha = 1 ` thì sẽ chuyển đổi dữ liệu tại cạnh đầu tiên của xung clock và đọc dữ liệu tại cạnh thứ 2 của xung clock. 

Kết hợp cả 4 cấu hình thì ta sẽ có 4 chế độ hoạt động cho SPI:

- Mode 0 (CPOL = 0 , CPHA = 0): Đọc dữ liệu tại cạnh lên chuyển đổi dữ liệu tại cạnh xuống.
- Mode 1 (CPOL = 0 , CPHA = 1): Đọc dữ liệu tại cạnh xuống chuyển đổi dữ liệu tại cạnh lên.
- Mode 2 (CPOL = 1 , CPHA = 0): Đọc dữ liệu tại cạnh xuống chuyển đổi dữ liệu tại cạnh lên.
- Mode 3 (CPOL = 1 , CPHA = 1): Đọc dữ liệu tại cạnh lên chuyển đổi dữ liệu tại cạnh xuống.

#### 3. I2C:
#### - Định nghĩa: 
- Giao thức (Inter-Intergrated Circuit): đây là một chuẩn giao tiếp nối tiếp, có thể giao tiếp đồng bộ, nhưng chỉ hoạt động được 1 chế độ là bán song công, một master có thể giao tiếp với nhiều slave, so với SPI thì I2C chỉ sử dụng 2 dây giao tiếp ít hơn 2 dây khi mắc dây thì tất cả chân sda của master và slave đều mắc chung với nhau tương tự cho chân scl. Bus của I2C hoạt động ở chế độ open drain do đó để tránh tín hiệu bị floating thì ta sẽ mắc với điện trở kéo lên.

I2C sử dụng 2 dây để giao tiếp:

- SCL (Serial Clock): Chân tín hiệu xung clock được tạo bởi Master truyền đến cho slave.
- SDA (Serial Data):  Chân chứa dữ liệu được truyền đi.
#### - Quá trình truyền I2C:
- Để bắt đầu quá trình truyền dữ liệu thì master đang ở chế độ nghỉ(chân sda và chân scl đều đang ở mức cao) thì master sẽ kéo chân sda về mức 0 trước khi kéo chân scl, dữ liệu đầu tiên mà master sẽ truyền là 7 bit địa chỉ của slave mà master chọn truyền hoặc là nhận cộng với 1 bit là chọn chế độ truyền hoặc là nhận, trên i2C thì tất cả các slave sẽ có một địa chỉ riêng để master chọn slave nào để giao tiếp, sau khi slave đã nhận địa chỉ đó thì sẽ xác nhận địa chỉ đó có giống với địa chỉ của slave đó không nếu không thì sẽ không có hành động nào xảy ra nếu có thì sẽ gửi một ACK tới master (tức là sẽ kéo chân `sda = 0`) để xác nhận với master là có slave này để truyền hoặc nhận dữ liệu. 
- Master truyền dữ liệu cho slave: Sau khi xác nhận địa chỉ xong thì master sẽ bắt đầu quá trình truyền 8 bit dữ liệu cho slave sau khi slave đã nhận đủ dữ liệu thì slave sẽ gửi ack tới master xác nhận là đã nhận dữ liệu.    
- Master nhận dữ liệu từ slave: Sau khi xác nhận địa chỉ xong thì slave sẽ bắt đầu quá trình truyền 8 bit dữ liệu cho master sau khi master đã nhận đủ dữ liệu thì master sẽ gửi ack tới slave xác nhận là đã nhận dữ liệu.
- Sau khi hoàn tất quá trình truyền nhận thì chân sda sẽ kéo lên là mức 1 trước khi kéo chân scl cùng mức để xác nhận là quá trình truyền nhận dữ liệu đã kết thúc. 
- Quá trình truyền dữ nhận địa chỉ và dữ liệu trong giao thức I2C khi 1 bit truyền đi đều sẽ kèm theo xung clock để xác nhận thời điểm truyền nhận giống với SPI.
#### 4. UART:
#### - Định nghĩa:
-  UART (Universal Asynchronous Receiver-Transmitter): đây là một chuẩn giao tiếp nối tiếp, khác với SPI và I2C thì UART giao tiếp không đồng bộ, không có phân cấp master và slave, chỉ có 2 thiết bị giao tiếp, hoạt động chế độ đơn công hoặc song công sử dụng 2 dây để giao tiếp Tx và Rx, Tx của thiết bị 1 sẽ nối với Rx của thiết bị 2, Rx của thiết bị 1 nối với Tx của thiết bị 2.
UART sử dụng 2 dây để giao tiếp: 
- Tx (Transmit): Chân truyền dữ liệu 
- Rx (Receive):  Chân nhận dữ liệu.
#### - Quá trình truyền dữ liệu UART: 
- Để bắt đầu quá trình truyền dữ liệu thì MCU A sẽ kéo chân TX từ mức 1 xuống 0, để gửi 1 bit dữ liệu thì MCU A phải canh thời gian để gửi dữ liệu và MCU B cũng phải canh thời gian để nhận dữ liệu 2 thiết bị, sẽ phải canh thời gian cùng một lúc tức là MCU A dùng timer canh thời gian truyền 1 bit thì MCU B cũng dùng timer canh thời gian để nhận 1 bit, 2 timer của 2 thiết bị phải có thời gian canh bằng nhau, trong trường hợp 2 thiết bị cùng xung clock để dùng timer cùng tham số truyền vào thì sẽ canh thời gian bằng nhau nhưng ở trường hợp khác khi có 1 thiết bị khác dòng với thiết bị 2 thì thiết bị 1 sẽ có xung clock cao hơn thiết bị 2 thì khi truyền cùng một tham số vào timer thì 2 thiết bị sẽ có timer khác nhau khiến cho dữ liệu truyền đi bị sai để tránh việc này xảy ra ta sẽ dùng thông số baudrate để có thời gian canh bằng nhau, bất kỳ thiết bị từ dòng nào khi dùng cùng tham số truyền vào baudrate thì sẽ có thời gian canh truyền nhận bằng nhau.
- Baudrate: được định nghĩa là số bit truyền được trong 1 giây ta có thể set baudrate 9600, 19200, 38400... bits trên giây.
- Như đã nói ở trên thì để truyền 1 bit dữ liệu thì ta sẽ canh thời gian bằng nhau của 2 thiết bị để truyền và nhận cùng 1 lúc thay vì dùng timer để canh thì ta sẽ dùng thông số baudrate. Quá trình lặp lại liên tục canh thời gian truyền nhận cho mấy bit dữ liệu tiếp theo.
- Ta có thể kiểm tra dữ liệu sau khi nhận có đúng hay không bằng cách truyền parity bit bằng cách cộng hoặc không với 1 bit để phù hợp qui luật chẵn (đếm tất cả số bit 1 là số chẵn) hoặc qui luật lẻ (đếm tất cả các bit 1 là số lẻ ) cụ thể:
Qui luật chẵn: 
- Data khi chuyền là 0b0000 0000 áp dụng qui luật chẵn 0b 0000 0000 0.
- Data khi chuyền là 0b0011 0100 áp dụng qui luật chẵn 0b 0011 0100 1.
- Data khi chuyền là 0b1111 1111 áp dụng qui luật chẵn  0b1111 1111 0.
Qui luật lẻ:
- Data khi chuyền là 0b0000 0000 áp dụng qui luật lẻ 0b 0000 0000 1.
- Data khi chuyền là 0b0011 0100 áp dụng qui luật lẻ 0b 0011 0100 0.
- Data khi chuyền là 0b1111 1111 áp dụng qui luật lẻ 0b1111 1111 1.
 Ví dụ dữ liệu truyền đi là 0b 0100 0100 khi áp dụng qui luật chẵn là 0b 0100 0100 0 (đếm tổng cộng có 2 bit) là số chẵn nhưng khi nhận dữ liệu là 0b 0000 0100 0 (đếm tổng cộng có 1 bit) là số lẻ thì kết luận là dữ liệu nhận bị sai.
- Parity có nhược điểm là chỉ kiểm tra với số lượng bit lỗi là số lẻ, không đúng số lượng bit lỗi là số chẵn.
- Sau khi truyền xong dữ liệu thì MCU A sẽ kéo chân Tx từ 0 lên 1.
