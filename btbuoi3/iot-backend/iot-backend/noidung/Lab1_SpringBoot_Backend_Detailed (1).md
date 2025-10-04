# 🧪 Lab 1 (Chi tiết): Khởi tạo Backend Spring Boot + CSDL (PostgreSQL/MySQL)

> **Mục tiêu cuối buổi:** Chạy được API `GET /api/devices` trả về danh sách thiết bị lưu trong DB.

---

## 📦 0) Tổng quan kiến trúc & phạm vi Lab
```
Thiết bị (giả lập ở Lab sau) 
        │  publish MQTT
        ▼
   MQTT Broker (Lab 2)
        │  subscribe
        ▼
 Backend Spring Boot  ──► PostgreSQL/MySQL ──► Dữ liệu devices/telemetry
        ▲
        │  REST API (GET /api/devices)
        └── Client (Postman/cURL/Frontend)
```
Trong **Lab 1**, ta chỉ tập trung vào **Backend + Database** và **API GET**.

---

## ✅ 1) Kiểm tra môi trường & cài đặt
Yêu cầu:
- **Java 17+**, **Maven 3.8+**
- IDE: IntelliJ IDEA / VS Code
- **Docker** (khuyến nghị, để chạy DB nhanh)
- Postman/cURL để test API

### Kiểm tra phiên bản
```bash
java -version
mvn -v
docker -v   # nếu dùng Docker
```

> Nếu chưa có, cài đặt:
> - Java: Temurin/OpenJDK 17
> - Maven: https://maven.apache.org/download.cgi
> - Docker Desktop (Windows/Mac) hoặc Docker Engine (Linux)

---

## 🚀 2) Tạo mới project Spring Boot

### Cách A — Spring Initializr (UI)
1. Truy cập <https://start.spring.io/>
2. Chọn:
   - **Project:** Maven
   - **Language:** Java
   - **Spring Boot:** 3.x
   - **Group:** `com.example`
   - **Artifact:** `iot-backend`
3. **Dependencies:**
   - Spring Web
   - Spring Data JPA
   - PostgreSQL Driver *(hoặc MySQL Driver nếu dùng MySQL)*
   - Lombok *(tùy chọn)*
4. Nhấn **Generate** để tải về file `.zip`, sau đó **giải nén**.

### Cách B — Dòng lệnh (curl)
```bash
curl https://start.spring.io/starter.zip \
  -d language=java -d type=maven-project -d bootVersion=3.3.4 \
  -d groupId=com.example -d artifactId=iot-backend \
  -d dependencies=web,data-jpa,postgresql,lombok \
  -o iot-backend.zip

unzip iot-backend.zip -d iot-backend
cd iot-backend
```

> **Tip:** Nếu dùng MySQL, đổi `postgresql` → `mysql` trong tham số `dependencies`.

---

## 🧭 3) Cấu trúc thư mục chuẩn (sau khi tạo)
```
iot-backend/
 ├─ src/main/java/com/example/iotbackend/
 │   ├─ IotBackendApplication.java
 │   ├─ controller/
 │   ├─ service/
 │   ├─ repository/
 │   └─ model/           # (hoặc entity/)
 ├─ src/main/resources/
 │   ├─ application.yml
 │   └─ data.sql         # seed dữ liệu mẫu (sẽ tạo ở bước sau)
 ├─ pom.xml
 └─ ...
```

---

## 🗄️ 4) Khởi chạy Database

### 4.1 PostgreSQL bằng Docker (khuyến nghị)
```bash
docker run -d --name pg-iot -p 5432:5432 \
  -e POSTGRES_USER=iotuser -e POSTGRES_PASSWORD=secret \
  -e POSTGRES_DB=iotdb postgres:16
```
**Kiểm tra kết nối (tùy chọn):**
```bash
docker exec -it pg-iot psql -U iotuser -d iotdb -c "\dt"
```

### 4.2 MySQL bằng Docker (nếu muốn)
```bash
docker run -d --name mysql-iot -p 3306:3306 \
  -e MYSQL_ROOT_PASSWORD=secret \
  -e MYSQL_DATABASE=iotdb \
  -e MYSQL_USER=iotuser -e MYSQL_PASSWORD=secret \
  mysql:8
```
**Kiểm tra kết nối (tùy chọn):**
```bash
docker exec -it mysql-iot mysql -uiotuser -psecret -e "SHOW DATABASES;"
```

> Nếu không dùng Docker, hãy cài PostgreSQL/MySQL bản native và tạo DB/user tương ứng.

---

## ⚙️ 5) Cấu hình `application.yml`
Tạo/sửa `src/main/resources/application.yml`:

```yaml
spring:
  datasource:
    url: jdbc:postgresql://localhost:5432/iotdb
    username: iotuser
    password: secret
  jpa:
    hibernate:
      ddl-auto: update
    properties:
      hibernate:
        dialect: org.hibernate.dialect.PostgreSQLDialect
        format_sql: true
    show-sql: true
  sql:
    init:
      mode: always
  jpa:
    defer-datasource-initialization: true

server:
  port: 8080
```

> **Dùng MySQL?** Đổi:
> ```yaml
> spring:
>   datasource:
>     url: jdbc:mysql://localhost:3306/iotdb?useSSL=false&allowPublicKeyRetrieval=true&serverTimezone=UTC
>     username: iotuser
>     password: secret
>   jpa:
>     hibernate:
>       ddl-auto: update
>     properties:
>       hibernate:
>         dialect: org.hibernate.dialect.MySQLDialect
>     show-sql: true
>   sql:
>     init:
>       mode: always
>   jpa:
>     defer-datasource-initialization: true
> ```

---

## 🧱 6) Tạo Entity (JPA)

### 6.1 `Device.java`
`src/main/java/com/example/iotbackend/model/Device.java`
```java
package com.example.iotbackend.model;

import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "devices")
public class Device {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;

  @Column(nullable=false, length=100)
  private String name;

  @Column(length=50)
  private String type;   // sensor, actuator, gateway...

  @Column(length=20)
  private String status; // ONLINE/OFFLINE/UNKNOWN

  private Instant createdAt = Instant.now();

  public Long getId() { return id; }
  public void setId(Long id) { this.id = id; }
  public String getName() { return name; }
  public void setName(String name) { this.name = name; }
  public String getType() { return type; }
  public void setType(String type) { this.type = type; }
  public String getStatus() { return status; }
  public void setStatus(String status) { this.status = status; }
  public Instant getCreatedAt() { return createdAt; }
  public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
}
```

### 6.2 `Telemetry.java` (chuẩn bị cho Lab 4)
`src/main/java/com/example/iotbackend/model/Telemetry.java`
```java
package com.example.iotbackend.model;

import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "telemetry")
public class Telemetry {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;

  @ManyToOne(optional=false)
  @JoinColumn(name="device_id")
  private Device device;

  private Instant ts = Instant.now();

  @Column(columnDefinition = "text")
  private String data; // JSON string: {"temp": 26.5, "hum": 70}

  public Long getId() { return id; }
  public void setId(Long id) { this.id = id; }
  public Device getDevice() { return device; }
  public void setDevice(Device device) { this.device = device; }
  public Instant getTs() { return ts; }
  public void setTs(Instant ts) { this.ts = ts; }
  public String getData() { return data; }
  public void setData(String data) { this.data = data; }
}
```

> **Mẹo:** Có thể dùng **Lombok** (`@Data`, `@NoArgsConstructor`, `@AllArgsConstructor`) để rút gọn getter/setter.

---

## 📚 7) Repository
`src/main/java/com/example/iotbackend/repository/DeviceRepository.java`
```java
package com.example.iotbackend.repository;

import com.example.iotbackend.model.Device;
import org.springframework.data.jpa.repository.JpaRepository;

public interface DeviceRepository extends JpaRepository<Device, Long> {
}
```

---

## 🧠 8) Service
`src/main/java/com/example/iotbackend/service/DeviceService.java`
```java
package com.example.iotbackend.service;

import com.example.iotbackend.model.Device;
import com.example.iotbackend.repository.DeviceRepository;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class DeviceService {
  private final DeviceRepository repo;

  public DeviceService(DeviceRepository repo) {
    this.repo = repo;
  }

  public List<Device> findAll() {
    return repo.findAll();
  }
}
```

---

## 🌐 9) Controller (API `GET /api/devices`)
`src/main/java/com/example/iotbackend/controller/DeviceController.java`
```java
package com.example.iotbackend.controller;

import com.example.iotbackend.model.Device;
import com.example.iotbackend.service.DeviceService;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/devices")
public class DeviceController {
  private final DeviceService service;

  public DeviceController(DeviceService service) {
    this.service = service;
  }

  @GetMapping
  public List<Device> getAllDevices() {
    return service.findAll();
  }
}
```

---

## 🌱 10) Seed dữ liệu mẫu
Tạo file `src/main/resources/data.sql`:
```sql
INSERT INTO devices (name, type, status, created_at) VALUES
  ('ESP32 Living Room', 'sensor', 'ONLINE', now()),
  ('Raspberry Pi 4', 'gateway', 'OFFLINE', now()),
  ('Smart Plug A1', 'actuator', 'ONLINE', now());
```

> **Lưu ý:** Đảm bảo trong `application.yml` đã bật:
> - `spring.sql.init.mode=always`
> - `spring.jpa.defer-datasource-initialization=true`

---

## ▶️ 11) Chạy ứng dụng
Từ thư mục gốc dự án:
```bash
./mvnw spring-boot:run        # macOS/Linux
# hoặc
mvnw.cmd spring-boot:run      # Windows
```

Xem log: Hibernate sẽ tạo bảng `devices`, `telemetry` và chạy `data.sql`.

---

## 🔎 12) Kiểm thử API
### Dùng cURL
```bash
curl -s http://localhost:8080/api/devices | jq .
```
*(bỏ `| jq .` nếu không có jq)*

### Kết quả mong đợi (ví dụ)
```json
[
  {
    "id": 1,
    "name": "ESP32 Living Room",
    "type": "sensor",
    "status": "ONLINE",
    "createdAt": "2025-09-15T10:00:00Z"
  },
  {
    "id": 2,
    "name": "Raspberry Pi 4",
    "type": "gateway",
    "status": "OFFLINE",
    "createdAt": "2025-09-15T10:00:00Z"
  }
]
```

### Dùng Postman
- Method: **GET**
- URL: `http://localhost:8080/api/devices`
- Send → xem JSON trả về.

---

## 🧩 13) (Tuỳ chọn) Dùng Flyway để quản lý schema
**Thêm dependency vào `pom.xml`:**
```xml
<dependency>
  <groupId>org.flywaydb</groupId>
  <artifactId>flyway-core</artifactId>
</dependency>
```
**Tạo file migration** `src/main/resources/db/migration/V1__init.sql`:
```sql
CREATE TABLE IF NOT EXISTS devices (
  id BIGSERIAL PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  type VARCHAR(50),
  status VARCHAR(20),
  created_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS telemetry (
  id BIGSERIAL PRIMARY KEY,
  device_id BIGINT NOT NULL REFERENCES devices(id),
  ts TIMESTAMP,
  data TEXT
);
```
**Đổi cấu hình JPA:**
```yaml
spring:
  jpa:
    hibernate:
      ddl-auto: none   # giao cho Flyway quản lý schema
```

---

## 🛠️ 14) Lỗi thường gặp & cách khắc phục
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| `Connection refused` | DB chưa chạy / sai URL/port/username/password | Kiểm tra container DB, thông số kết nối |
| Không tạo bảng | `ddl-auto` không bật (hoặc dùng Flyway nhưng chưa tạo migration) | Bật `ddl-auto: update` (Lab) hoặc thêm migration |
| `data.sql` không chạy | Thiếu `spring.jpa.defer-datasource-initialization=true` | Bổ sung cấu hình này |
| Cổng 8080 bận | Ứng dụng khác đang chạy | Đổi `server.port` trong `application.yml` |
| Lỗi driver MySQL/PostgreSQL | Sai dependency | Kiểm tra `pom.xml`, chọn đúng driver DB đang dùng |

---

## 🏁 Kết quả cuối cùng
- Dự án Spring Boot khởi chạy OK và kết nối CSDL thành công.  
- Bảng `devices`, `telemetry` đã được tạo.  
- API **`GET /api/devices`** trả JSON danh sách thiết bị từ DB.

---

## 📚 Bài tập củng cố (5–10 phút)
1) Thêm API `GET /api/devices/{id}` trả về 1 thiết bị theo `id`.  
2) Thêm field `location` (VARCHAR 100) cho bảng `devices` và seed lại dữ liệu.  
3) Thêm sắp xếp theo `createdAt` giảm dần khi lấy danh sách.

> Gợi ý: dùng `JpaRepository#findAll(Sort.by("createdAt").descending())` trong Service.
