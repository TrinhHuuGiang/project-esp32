- Trong dự án có thể thấy các loại file `CMakeLists.txt`, `Makefile`, các file `.mk`.
    - Ta chỉ cần cấu hình `CMakeLists.txt` còn đâu CMake tự tạo ra file còn lại.
- Cấu hình 1 dự án thường thấy:
```txt
- myProject/
             - CMakeLists.txt
             - sdkconfig
             - dependencies.lock
             - bootloader_components/ - boot_component/ - CMakeLists.txt
                                                        - Kconfig
                                                        - src1.c
             - components/ - component1/ - CMakeLists.txt
                                         - Kconfig
                                         - src1.c
                           - component2/ - CMakeLists.txt
                                         - Kconfig
                                         - src1.c
                                         - include/ - component2.h
             - managed_components/ - namespace__component-name/ - CMakelists.txt
                                                                - src1.c
                                                                - idf_component.yml
                                                                - include/ - src1.h
             - main/       - CMakeLists.txt
                           - src1.c
                           - src2.c
                           - idf_component.yml
             - build/
```
- Qui tắc của `CMakeLists.txt`
    - Đầu tiên nó sẽ chạy `CMakeLists.txt` mặc định ở ngoài cùng.
    - Khi này `include($ENV{IDF_PATH}/tools/cmake/project.cmake)` sẽ tìm đến các file `CMakeLists.txt` theo cấu trúc thư mục mặc định như trên, nếu có thể tìm thấy.
        - Lưu ý lệnh `project(main)` trong file `CMakeLists.txt` ở ngoài cùng nên được đổi tên `main` thành tên khác vì đây là lệnh tạo chương trình với tên là `main`.
    - Các file `CMakeLists.txt` con sẽ chứa:
        - Các file con bên trong thư mục ấy cần biên dịch.
        - các đường dẫn đến `CMakeLists.txt`.
        - lệnh đặc biệt:
        ```c
        idf_component_register(SRCS "main.c" // thêm danh sách các file .c
                    INCLUDE_DIRS ".") // thêm danh sách các file header
        ```
        - Dĩ nhiên nếu 1 file `.h` mà thiết đi file định nghĩa `.c` cho nó thì không thể chạy được. khi ấy ta cần thêm lệnh để tìm file `CMakeLists.txt` của chúng:
            - CHạy lệnh:
            ```c
            add_subdirectory(components/my_lib)  // Bắt buộc chạy `CMakeLists.txt` nằm trong `my_lib` chẳng hạn
            ```

- File `Kconfig` và `sdkconfig.h` và `sdfconfig`
    - Đơn giản thì nơi nào có `CMakeLists.txt` đề có thể cần 1 file `Kconfig`
    - File `Kconfig` có thể định nghĩa các biến có thể cấu hình từ `menuconfig` trước khi build 1 dự án.
    - Các biến `Kconfig` được quản lý tập trung bởi `sdfconfig`
    - Khi biến dịch chương trình `sdfconfig` sẽ tập trung các biến lại và sinh ra 1 thư viện là `sdfconfig.h`
    - Như vậy để dùng được các giá trị này, các file code cần được include thêm `sdfconfig.h`
    - Các giá trị trong `sdfconfig.h` sẽ là các macro #define được tiền xử lý trước khi chạy code.
    - Như vậy cần đặt tên các biến trong `Kconfig` cẩn thận tránh trùng lặp.