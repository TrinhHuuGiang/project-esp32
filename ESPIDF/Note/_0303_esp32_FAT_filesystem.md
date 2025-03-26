# FatFs
- FatFs là hệ thống file FAT (File Allcation Table) dùng để quản lý kết nối (USP, thẻ SD, ổ cứng, hệ thống nhúng...) được đăng ký vào hệ thống file ảo VFS của esp32 (VFS là hệ thống quản lý file thông qua các hàm chuẩn C và chuẩn POSIX)
- Các bước kết nối thẻ với hệ thống file thông qua `spi`:
    1. gọi `esp_vfs_fat_register()` đăng ký một đường dẫn đại diện cho ổ đĩa ảo trên esp32 mà thẻ sẽ được ánh xạ vào.
    2. gọi `ff_diskio_register()` để đăng ký driver cho ổ đĩa ảo và lấy thông tin điều khiển ổ đĩa ảo.
    - *Không cần làm bước 1 và 2 hãy làm luôn bước 3.*
    3. Đặc biệt hãy gọi `esp_vfs_fat_sdspi_mount` có chức năng thực hiện luôn bước 1 và 2: 
        - Khởi tạo `SPI master device config` thông qua `slot_config` và tự đăng ký nó với bus spi.
        - Khởi tạo cấu hình thẻ SD trong `host_config_input`
        - Liên kết phân vùng FAT trên thẻ SD bằng thư viện FatFs, cấu trình trong `mount_config`
    4. Sử dụng các hàm chuẩn C và POSIX để giao tiếp:
        - Lưu ý đương dẫn tới file phải bao gồm đường dẫn đại diện cho hệ thông đã đăng ký ở trước.
            - VD: `/sdcard` là đường dẫn đại diện thì file là `/sdcard/filename.type`
        - Lưu ý bật `CONFIG_FATFS_LONG_FILENAMES` trong menuconfig nếu cần dùng tên dài
        - Mặc định espidf sử dụng hệ thống tên file `8.3 filename`:
            - `8.3 filename` còn gọi là a short file name `SFN` . [wiki link](https://en.wikipedia.org/wiki/8.3_filename).
            - `prefix` của đường dẫn có thể dài nhưng file phải tuân thủ qui tắc đặt tên:
                - `8` là 8 ký tự cho tên
                - `.3` là 3 ký tự cho phần mở rộng
            - hệ thống tên file này không phân biệt hoa thường:
                - khi ghi file mặc định tạo ra file có tên in hoa ví dụ `FILENAME.TXT`
                - khi đọc file không quan trọng hoa thường
                - thực tế khi tạo file trên các hệ thống FAT như Window ta cũng không thể tạo 2 tên giống nhưng chỉ khác về hoa thường.
                    - ví dụ `File.txt` vẫn là `file.txt` và hệ thống Window hỏi muốn xóa 1 trong 2
    5. Đóng file sau khi dùng xong
    6. Dùng `esp_vfs_fat_sdcard_unmount()` để ngắt liên kết `un_mount`
    - *Không cần làm bước 7 và 8 vì bước 6 đã làm rồi*
    7. Dùng `ff_diskio_register()` để xóa driver I/O
    8. Dùng `esp_vfs_fat_unregister_path()` để gỡ đường dẫn ra khỏi hệ thống VFS