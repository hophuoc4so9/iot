# 🧪 Lab 3 (Chi tiết): Quản lý Thiết bị IoT (CRUD API)

> **Mục tiêu cuối buổi:** Xây dựng được API REST cơ bản để **thêm, sửa, xóa, lấy danh sách thiết bị** trong hệ thống IoT.  
> **Mở rộng (tuỳ chọn):** Thêm **JWT authentication** để bảo mật API.

---

## 🧭 0) Ngữ cảnh & phạm vi Lab
Trong Lab 1, ta đã tạo entity `Device` và API `GET /api/devices`.  
Trong Lab 3 này, ta mở rộng để có đủ **CRUD API**:  
- `GET /api/devices` → Lấy danh sách  
- `POST /api/devices` → Thêm mới  
- `PUT /api/devices/{id}` → Cập nhật  
- `DELETE /api/devices/{id}` → Xóa  

```
Client (Postman / Frontend) ──► REST API ──► Backend ──► Database
```

---

## ✅ 1) Chuẩn bị
- Code từ Lab 1 (đã có entity `Device`, repo, service, controller)
- DB PostgreSQL/MySQL đã chạy
- Postman/cURL để test API

---

## 🧱 2) Cập nhật Service để hỗ trợ CRUD

`src/main/java/com/example/iotbackend/service/DeviceService.java`

```java
package com.example.iotbackend.service;

import com.example.iotbackend.model.Device;
import com.example.iotbackend.repository.DeviceRepository;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class DeviceService {
  private final DeviceRepository repo;

  public DeviceService(DeviceRepository repo) {
    this.repo = repo;
  }

  public List<Device> findAll() {
    return repo.findAll();
  }

  public Optional<Device> findById(Long id) {
    return repo.findById(id);
  }

  public Device save(Device device) {
    return repo.save(device);
  }

  public void delete(Long id) {
    repo.deleteById(id);
  }
}
```

---

## 🌐 3) Controller cho CRUD API

`src/main/java/com/example/iotbackend/controller/DeviceController.java`

```java
package com.example.iotbackend.controller;

import com.example.iotbackend.model.Device;
import com.example.iotbackend.service.DeviceService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/devices")
public class DeviceController {
  private final DeviceService service;

  public DeviceController(DeviceService service) {
    this.service = service;
  }

  // GET: danh sách thiết bị
  @GetMapping
  public List<Device> getAllDevices() {
    return service.findAll();
  }

  // GET: 1 thiết bị theo id
  @GetMapping("/{id}")
  public ResponseEntity<Device> getDeviceById(@PathVariable Long id) {
    return service.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
  }

  // POST: thêm mới thiết bị
  @PostMapping
  public Device createDevice(@RequestBody Device device) {
    return service.save(device);
  }

  // PUT: cập nhật thiết bị
  @PutMapping("/{id}")
  public ResponseEntity<Device> updateDevice(@PathVariable Long id, @RequestBody Device device) {
    return service.findById(id)
        .map(existing -> {
          existing.setName(device.getName());
          existing.setType(device.getType());
          existing.setStatus(device.getStatus());
          return ResponseEntity.ok(service.save(existing));
        })
        .orElse(ResponseEntity.notFound().build());
  }

  // DELETE: xóa thiết bị
  @DeleteMapping("/{id}")
  public ResponseEntity<Void> deleteDevice(@PathVariable Long id) {
    if (service.findById(id).isPresent()) {
      service.delete(id);
      return ResponseEntity.noContent().build();
    } else {
      return ResponseEntity.notFound().build();
    }
  }
}
```

---

## ▶️ 4) Test CRUD bằng Postman/cURL

### 4.1 GET danh sách
```bash
curl -s http://localhost:8080/api/devices | jq .
```

### 4.2 POST thêm thiết bị
```bash
curl -X POST http://localhost:8080/api/devices \
  -H "Content-Type: application/json" \
  -d '{"name":"ESP8266 Kitchen","type":"sensor","status":"ONLINE"}'
```

### 4.3 PUT cập nhật thiết bị
```bash
curl -X PUT http://localhost:8080/api/devices/1 \
  -H "Content-Type: application/json" \
  -d '{"name":"ESP8266 Kitchen","type":"sensor","status":"OFFLINE"}'
```

### 4.4 DELETE xóa thiết bị
```bash
curl -X DELETE http://localhost:8080/api/devices/1 -v
```

Kỳ vọng:
- POST trả về JSON thiết bị mới tạo (có id).  
- PUT trả về JSON thiết bị đã cập nhật.  
- DELETE trả về `204 No Content`.  

---

## 🔒 5) (Mở rộng) Bảo mật API với JWT

### 5.1 Thêm dependency
`pom.xml`
```xml
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-security</artifactId>
</dependency>
<dependency>
  <groupId>io.jsonwebtoken</groupId>
  <artifactId>jjwt-api</artifactId>
  <version>0.11.5</version>
</dependency>
<dependency>
  <groupId>io.jsonwebtoken</groupId>
  <artifactId>jjwt-impl</artifactId>
  <version>0.11.5</version>
  <scope>runtime</scope>
</dependency>
<dependency>
  <groupId>io.jsonwebtoken</groupId>
  <artifactId>jjwt-jackson</artifactId>
  <version>0.11.5</version>
  <scope>runtime</scope>
</dependency>
```

### 5.2 Ý tưởng triển khai (tóm tắt)
1. Tạo endpoint `/auth/login` để sinh JWT khi user login.  
2. Tạo class `JwtFilter` để kiểm tra JWT trong header `Authorization: Bearer ...`.  
3. Cấu hình Spring Security để bảo vệ các API `/api/devices/**`.  
4. Chỉ user có token hợp lệ mới được CRUD thiết bị.

*(Chi tiết sẽ được làm rõ trong Lab bảo mật riêng.)*

---

## 🛠️ 6) Lỗi thường gặp
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| POST/PUT trả 400 Bad Request | Sai JSON body | Kiểm tra JSON gửi đi, thêm header `Content-Type: application/json` |
| DELETE trả 404 | id không tồn tại | Kiểm tra id có trong DB chưa |
| Hibernate không tạo bảng | Thiếu cấu hình `ddl-auto=update` | Sửa `application.yml` |
| API trả 403 khi bật JWT | Chưa gửi token / token sai | Đăng nhập lấy JWT trước rồi gắn vào header |

---

## 🏁 Kết quả cuối cùng
- Đã có đầy đủ API REST để quản lý thiết bị: GET, POST, PUT, DELETE.  
- Có thể quản lý danh sách thiết bị từ Postman/Frontend.  
- (Mở rộng) Có thể thêm bảo mật JWT cho API.

---

## 📚 Bài tập củng cố
1) Thêm field `location` cho `Device`, cập nhật CRUD API để xử lý.  
2) Thêm API `GET /api/devices/search?status=ONLINE` để tìm theo trạng thái.  
3) Thêm validate: `name` không được rỗng, `status` chỉ nhận `ONLINE/OFFLINE`.  
4) (Nâng cao) Bật JWT bảo vệ API, yêu cầu user phải login trước khi thao tác.  
