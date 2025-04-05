# Nội dung

## Giới thiệu
- Đầu tiên đề tập đến RIFF (Resource interchange file format) là một định dạng file lưu dữ và dư liệu được biểu diễn theo các `chunk`.
- Các `chunk` gồm: `FourCC` tag phải là  4byte toàn các chữ in hoa, `size` của data tính theo byte của 1 chunk, và `data` đi kèm của chunk.
- Các `data` có thể là dữ liệu hoặc các `subchunk` nhỏ hơn. `FourCC` tag của `subchunk` không cần viết in hoa.
- File WAV là một `intance` (đối tượng) trong số nhiều loại định dạng file khác mà RIFF hỗ trợ.
- WAV file (Waveform audio file format) còn gọi là WAVE là chuẩn định dạng tệp lưu trữ âm thanh được phát triển
lần đầu vào 1991 bởi IBM và Microsoft.
- Đây là chuẩn âm thanh không nén, luồng bit được đều chế theo phương pháp điều chế xung mã tuyến tính (LPCM) với các
mức lượng tự hóa là tuyến tính.

## Cấu trúc
- `chunk` 