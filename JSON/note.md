# JSON - JavaScript Object Notation
- JSON là một định dạng văn bản dùng để lưu trữ và trao đổi dữ liệu.
- Trong JavaScript, một JSON luôn ở dạng chuỗi ký tự khi chưa được parse thành kiểu dữ liệu khác.
- JSON độc lập với ngôn ngữ lập trình, có thể được sử dụng trên nhiều nền tảng khác nhau.
- JSON có tính chất self-describing, tức là có thể đọc và hiểu dễ dàng.
- Tệp JSON có phần mở rộng .json.

## Cú pháp
1. Các dữ liệu trong JSON được viết dưới dạng các đối tượng {name:value}:
    - `name` là một chuỗi đại diện cho 1 dữ liệu nào đó.
    - Theo sau là `:` và `value` đại điện cho giá trị.
    - Các định dạng của `value` hợp lệ là:
        - `value` là chuỗi
            - ví dụ: {"Name":"Jame"}
        - `value` là số nguyên hoặc số thực
            - ví dụ: {"Number": 1}, các số có thể là 10, 3.14, -5
        - `value` là các `thuộc tính` được khác được cách bởi `,`
            - ví dụ:
            - ```JSON
              {"Object_name":
                  {
                      "name1":"value1",
                      "name2":value2
                  }
              }
              ```
        - `value` là `mảng` (array), có các các định dạng khác nhau, phân cách bởi `,`
            - ví dụ: {"Number":[true,fale,2,3,"bon","nam", {"name":"value}]}
            - mảng có thể gồm các loại value ở trên và chính đối tượng JSON.
        - `value` là `boolean` nhận `true` hoặc `false`
            - ví dụ: {"Bool":true}
        - null
            - ví dụ: {"Null":null}
2. Trong một số ngôn ngữ có thể gán trực tiếp giá trị chúng ta có khái niệm `JSON object literal`
    - `JSON object literal` là cặp `{` `}` ngoài cùng đại diện cho một đối tượng JSON không cần có `name` để `value` có thể gán trực tiếp.
        - ví dụ:
        ``` json
        {
            "object1":
            {
                "name":"Alice",
                "age":"10"
            },
            "name2":"Bob"
        }

        <!-- hoặc là -->
        {"Bool":true}

        ```
3. Tương tự chúng ta cũng có thể dùng `[` `]` làm đại diện cho mảng JSON
    - sự khác biệt ở cách mảng lưu được cả giá trị hoặc object còn `JSON object literal` chỉ lưu cặp `name`:`value`
    - mảng sử dụng khi chúng ta không cần thiết tạo `name` để lưu giá trị. Giúp mã ngắn gọn hơn.
