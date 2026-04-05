# JSON - JavaScript Object Notation
- JSON là một định dạng văn bản dùng để lưu trữ và trao đổi dữ liệu.
- JSON độc lập với ngôn ngữ lập trình, có thể được sử dụng trên nhiều nền tảng khác nhau.
- JSON có tính chất self-describing, tức là có thể đọc và hiểu dễ dàng.
- Tệp JSON có phần mở rộng .json.

## Cú pháp

1. Dữ liệu trong JSON thường được biểu diễn dưới dạng object với cặp thuộc tính - giá trị viết trong { }

    Ví dụ một object có thuộc tính "name":"value":

    {"name":"value"}

    - "name" là tên thuộc tính
    - Sau dấu : là giá trị của thuộc tính
    - Giá trị có thể là:

    - Chuỗi
        {"Name":"James"}

    - Số nguyên hoặc số thực
        {"Number":1}
        {"Pi":3.14}

    - Một object khác
        {
            "Object_name":
            {
                "name1":"value1",
                "name2":"value2"
            }
        }

        Trong ví dụ trên:
        - object ngoài cùng có 1 thuộc tính là "Object_name"
        - giá trị của "Object_name" là một object khác
        - object bên trong có 2 thuộc tính là "name1" và "name2"

    - Mảng (array)
        {
            "Number":[true,false,2,3,"bon","nam",{"name":"value"}]
        }

        Mảng có thể chứa:
        - số
        - chuỗi
        - boolean
        - object
        - mảng khác
        Các phần tử trong mảng được phân cách bởi dấu ,

    - Boolean
        {"Bool":true}

    - null
        {"Null":null}

2. JSON object literal

    JSON object literal là object ngoài cùng được viết trực tiếp bằng { }

    Ví dụ:

    {
        "name":"Alice",
        "age":"10"
    }

    hoặc

    {"Bool":true}

    Lưu ý: bên trong object literal vẫn luôn phải là các cặp:

        "name": value

3. Array literal

    Tương tự, ta cũng có thể dùng [ ] trực tiếp để biểu diễn một mảng:

    [1,2,3]

    hoặc

    [
       {"name":"Alice"},
        {"name":"Bob"}
    ]

Khác nhau:

- Object {} thường lưu dữ liệu 1 đối tượng theo tên thuộc tính
- Array [] thường lưu dữ liệu nhiều đối tượng theo thứ tự phần tử
