# Nội dung
- [Lập trình rule cho Firebase filestore](#firebase-security-rules-language)

# Firebase Security Rules language for Filestore
```js
service <<name>> {
  // Match the resource path.
  match <<path>> {
    // Allow the request if the following conditions are true.
    allow <<methods>> : if <<condition>>
  }
}
```
Cơ bản:
- `service`: Khai báo dịch vụ Firebase cloud muốn thiết lập rule (ví dụ cloud.firestore)
- `match`: Định nghĩa database hoặc đường dẫn lưu trữ cần áp dụng rule
- `allow`: cung cấp các điều kiện so sánh để được sử dụng các phương thức truy nhập `method`
- Standard methods: `get`, `list`, `create`, `update`, `delete`, `read`, `write`
- `function`: tùy chọn này cho phép nhóm các điều kiện lại và sử dụng lại trên nhiều qui tắc
- Một `service` có nhiều `match` để tùy chỉnh cho các đường dẫn khác nhau của database

Phiên bản:
```js
rules_version = '2';
service cloud.firestore {
...
}
```
- Mặc định không có `rules_version` thì dùng version 1

### Service
- Định nghĩa một loạt các luật thông qua `match` và sẽ áp dụng cho một dịch vụ duy nhất, cần phải chỉ ra dịch vụ sẽ áp dụng nó.
```js
service cloud.firestore {
 // Your 'match' blocks with their corresponding 'allow' statements and
 // optional 'function' declarations are contained here
}
```

### Match
- `match` khai báo kèm theo một đường dẫn `path` mà nó có thể được yêu cầu sau này từ phía người dùng thông qua `request.path`.
- một đường dẫn `path` theo sau là 1 thông số đường dẫn `{singleSegment}` hoặc nhiều thông số `{multiSegment=**}` để tự khớp với nhiều đường dẫn nằm trong tài nguyên Firebase cần truy cập.
- Nội dung trong `match` có thể bao gồm: `match` block khác, `allow` để kiểm tra điều kiện, khai báo `function`.
- Các `match` block con có đường dẫn tương đối so với đường dẫn `match` cha
- Các đường dẫn `path` có thể được khớp theo 2 kiểu:
    - Partial matches: `path` là 1 phần trùng khớp với `request.path`
        - khi điều này xảy ra hệ thống sẽ đi khớp tiếp `request.path` với `match path` con bên trong
        - điều này rất thích hợp ví dụ có `path` là `/users/` và user gửi 1 `request.path` là `/users/{userId}` vậy ta sẽ đi vào trong mục `user` và tìm rule ứng với id đó.
    - Complete matches: `path` trùng hoàn toàn với `request.path`
        - mục đích là nếu đến cuối dùng không thấy `match` con nào trùng thì từ chối luôn

```js
// Given request.path == /example/hello/nested/path
// Ví dụ request có path như trên
service firebase.storage {
  // Partial match. Ở đây {singleSegment} sẽ tự khớp với bất thì thanh phần đường dẫn con tiếp theo
  // ở đây là `hello`. Nhưng đường dẫn dài hơn nên sẽ khớp tiếp theo `Partial matches`
  match /example/{singleSegment} {   // `singleSegment` == 'hello'
    allow write;                     // Write rule not evaluated. 
    // Complete match.
    match /nested/path {             // `singleSegment` visible in scope.
      allow read;                    // Read rule is evaluated.
    }
  }
  // Complete match. {multiSegment=**} cho phép khớp mọi thành phần còn lại của đường dẫn
  match /example/{multiSegment=**} { // `multiSegment` == /hello/nested/path
    allow read;                      // Read rule is evaluated.
  }
}
```

### Allow

#### Method

#### Function