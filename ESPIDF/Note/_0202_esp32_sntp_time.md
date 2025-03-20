# Nội dung
- Thư viện: components/lwip/include/apps/esp_sntp.h
- Thêm thông qua CMakelists.txt: REQUIRE "esp_sntp.h"
- Các bước:
    - Cài đặt chế độ đồng bộ thời gian: `sntp_set_sync_mode()`
        - Người dùng chọn `SNTP_SYNC_MODE_IMMED` hoặc `SNTP_SYNC_MODE_SMOOTH`
            - Nếu chọn `SNTP_SYNC_MODE_IMMED` hàm sẽ tự động gọi `settimeofday()` để cập nhật ngay thời gian cho esp32 sau khi lấy được giờ từ hệ thống NTP (Network time protocol)
            - Nếu chọn `SNTP_SYNC_MODE_SMOOTH` hàm sẽ tự động gọi `adjtime()`
                - `adjtime()` sẽ quyết định có gọi `settimeofday()` chỉ khi thời gian chênh lệch quá 35 phút
    - Khởi tạo chế độ SNTP định kì:
        - `sntp_setoperatingmode(SNTP_OPMODE_POLL);`
        - chỉ có chế độ SNTP_OPMODE_POLL để lấy định kì sau 1 giờ
        - có thể chỉnh qua `CONFIG_LWIP_SNTP_UPDATE_DELAY`
    - Chọn server NTP:
        - Ví dụ: `sntp_setservername(0, "pool.ntp.org");`
    - Bắt đầu đồng bộ:
        - `sntp_init(); `
    - Điều chỉnh múi giờ để xem thời gian:
        - `setenv()`: cập nhật biến môi trường múi thời gian
        - `tzset()` : thay đổi hệ thống với múi giờ mới
    - Kết thúc đồng bộ:
        - thử tìm `sntp_stop()`

# Tips
- xem thư viện:` #include "esp_sntp.h"`