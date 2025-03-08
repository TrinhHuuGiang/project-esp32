# nội dung
- [Giới thiệu](#giới-thiệu)
- [Thông số](#thiết-kế)
- [Giao thức](#giao-tiếp)

# Giới thiệu
- I2C hay Inter-integrated Circuit, có tên gọi khác là IIC
- Giao tiếp multi-master/multi-slave
- Tốc độ thấp, giá thành rẻ và giao tiếp khoảng cách ngắn

# Thiết kế
- 2 chân tín hiệu: SDA (serial data line), SCL (serial clock line).
- Cả 2 chân tín hiệu đều cần có điện trở pull-up.
- Tốc độ tối đa giảm dần theo khoảng cách:
    - Chế dộ tốc độ thấp: 10kbps, vài mét
    - Tiêu chuẩn: 100kbps, khoảng ~1 mét truyền dẫn
    - Nhanh: 400kbps, ngắn hơn vào khoảng vài chục cm
    - cực nhanh: 3.4Mbps, cực ngắn thường tích hợp lên pcb
    - Ngoài ra còn một vài chuẩn tốc độ khác.
- Số lượng tối đa:
    - Master: không giới hạn
    - Slave: Thường thấy nhất trên thị trường là loại 7bit địa chỉ, ngoài ra còn có loại 10bit địa chỉ nhưng hiếm gặp.
        - Vì có nhiều địa chỉ đặc biệt nên số địa chỉ khả dụng khoảng ~100 địa chỉ 7bit
        - Các nhà sản xuất sẽ xử lý điều này

# Giao tiếp
- Khung dữ liệu:  
    [i2c_frame](img/_0101_i2c_frame.jpg)  
    - Điều kiện bắt đầu:
        - Các thiết bị slave đang trong trạng thái nghỉ
        - `SDA` đột nhiện bị kéo xuống thấp và đánh thức các thiết bị trong mạng.
        - Sau đó master tạo xung clock `SCL` kéo xuống thấp báo hiệu luồng dữ liệu bắt đầu được truyền lên đường dây.
    - Gửi `địa chỉ` và `1 bit R/w mode`:
        - 7bits địa chỉ sau đó được gửi lên đường dây
        - 1bit chọn chế độ đọc hoặc ghi được gửi đi
            - bit 0 là muốn truyền cho slave
            - bit 1 là muốn nhận dữ liệu
        - Cách gửi:
            - Ngay khi `SCL` ở trên vừa kéo xuống thì tín hiệu phải được gửi lên `SDA`
            - thời điểm `SCL` từ thấp lên cao chính là lúc lấy mẫu
            - Quá trình gửi lặp lại khi `SCL` tạo sườn xuống và lấy mẫu khi `SCL` tạo sườn lên.
    - ACK/NACK bit:
        - Nếu có 1 slave có địa chỉ trùng nó sẽ gửi 1 bit 0 lên đường truyền.
        - Nếu không có slave nào thì đường truyền luôn ở mức cao do điện trở pull-up
    - truyền nhận 8 bit dữ liệu:
        - sau đó là 8bit dữ liệu được trao đổi từ master đến slave hoặc từ slave về master tùy chế độ đọc ghi ở bit R/w mode.
    - ACK/NACK bit bên nhận:
        - sau khi nhận đủ thì bên nhận sẽ gửi ACK bằng cách kéo SDA về 0
        - nếu không thì vẫn để nguyên pull-up
    - Điều kiện kết thúc:
        - sau đó master có nhiệm vụ chuyển tín hiệu kết thúc lên đường SDA:
            - Nếu trước đó là ACK (0): tín hiệu kết thúc là khi `SCL` được kéo lên cao, sau đó `SDA` mới được kéo lên cao
            - Nếu trước đó là NACK (1): `SDA` không cần làm gì thêm cả