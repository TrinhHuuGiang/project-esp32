- Tạo projectL:
    - Trong thư mục cài esp-idf chọn thư mục `frameworks`
    - Chọn phiên bản và truy cập `examples`
    - Chọn `get-started` và copy `sample_project`

- Build project:
    - `cd` đến thư mục project
    - Chọn dòng chip esp32:
        - chạy lệnh `idf.py set-target TARGET`
        - mặc định  `TARGET` là chip `esp32`
        - xem các chip hỗ trợ bằng `idf.py set-target --help`
    - Cấu hình `menuconfig`:
        - Chạy `idf.py menuconfig` để mở menu cấu hình các tham số có thể đã được định sẵn trong code như wifi, mật khẩu, ... 
    - Biên dịch:
        - Chạy `idf.py build` để biên dịch chương trình.

- Nạp firmware:
    - Chạy `idf.py -p PORT [-b BAUD] flash`
    - `[-b BAUD]` là option có thể không có

- Xem log từ phần mềm:
    - chạy `idf.py -p PORT monitor` để xem log qua `uart`