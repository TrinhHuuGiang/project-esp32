# content
- [Giới thiệu](#giới-thiệu)
- [THông số](#thông-số-cấu-hình)
- [Tập lệnh](#tập-lệnh)

# Giới thiệu
- SSD1306 là chip CMOS driver dành cho OLED/PLED lại common cathode (âm chung). 
- Gồm 128 segment (đề điều khiển cột) và 64 commons (điều khiển hàng)
- SEG (segment) và COM (common) là khái niệm dùng trong ma trận điều khiển của led.
- Khả năng điều chỉnh độ tương phản của các pixel là 256 mức 
- Hỗ trợ giao tiếp: Song song, SPI, I2C
- Có RAM tích hợp lưu trạng thái các led và bộ dao động nội để luân phiên chốt dữ liệu khi quét màn hình

# Thông số cấu hình
- VCC: từ 7V đến 15V, dùng được VCC 5V
- VDD: điện áp dùng cho logic từ 1.65V - 3.3V
- Có thể cấu hình tốc độ khung hình
- nhiệt độ hoạt động -40 đến 85 độ C

# Tập lệnh
- Các cờ cần chú ý là:
    |Cờ D/C|Cờ R/W|
    |-|-|
    |Data (1) /Command (0)|Write (0) /Read (1)|
- Ta có bảng đọc ghi vào GDDRAM của ssd1306
    |D/C|R/W|Comment|Address auto increment|
    |0|0|Write Command|No|
    |0|1|Read Status|No|
    |1|0|Write Data|Yes|
    |1|1|Read Data|Yes|
- Command Table
