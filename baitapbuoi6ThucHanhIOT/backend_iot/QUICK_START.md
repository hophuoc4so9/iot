# 🚀 HƯỚNG DẪN CHẠY NHANH

## Bước 1: Kiểm tra yêu cầu

```powershell
# Kiểm tra Java (cần version 21+)
java -version

# Kiểm tra Maven (cần version 3.8+)
mvn -version

# Kiểm tra MySQL đang chạy
mysql -u root -p
```

Nếu chưa có, tải về:
- Java: https://adoptium.net/
- Maven: https://maven.apache.org/download.cgi
- MySQL: https://dev.mysql.com/downloads/mysql/

## Bước 2: Tạo Database

```sql
-- Đăng nhập MySQL
mysql -u root -p

-- Tạo database
CREATE DATABASE iot_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- Kiểm tra
SHOW DATABASES;

-- Thoát
exit;
```

## Bước 3: Cấu hình ứng dụng

Mở file `src/main/resources/application.properties` và sửa:

```properties
# Đổi password MySQL của bạn (nếu có)
spring.datasource.password=your_password_here

# Kiểm tra IP MQTT broker (mặc định: 10.15.150.248)
mqtt.broker=tcp://10.15.150.248:1883
```

## Bước 4: Chạy ứng dụng

### Cách 1: Dùng script (KHUYẾN NGHỊ)

```powershell
# Chạy file run.bat
.\run.bat

# Chọn option 3: Build and Run
```

### Cách 2: Dùng Maven trực tiếp

```powershell
# Build
mvn clean install

# Chạy
mvn spring-boot:run
```

## Bước 5: Kiểm tra hoạt động

Mở browser và truy cập:

```
http://localhost:8080/api/v1/info
```

Nếu thấy JSON response → **Thành công!** 🎉

## Bước 6: Test điều khiển LED

### Cách 1: Dùng giao diện web

1. Mở file `test-control-panel.html` trong browser
2. Click các nút màu để test
3. Xem logs và trạng thái

### Cách 2: Dùng cURL (PowerShell)

```powershell
# Đặt LED màu đỏ
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=255&g=0&b=0"

# Đặt LED màu xanh lá
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=0&g=255&b=0"

# Tắt LED
curl -X POST "http://localhost:8080/api/v1/device/led/off"

# Xem trạng thái
curl "http://localhost:8080/api/v1/device/state?deviceId=demo"
```

## ❌ Gặp lỗi?

### Lỗi: "Cannot connect to database"

```powershell
# Kiểm tra MySQL đang chạy
# Windows: Mở Services → tìm MySQL → Start

# Hoặc kiểm tra trong MySQL Workbench
```

Sửa password trong `application.properties`:
```properties
spring.datasource.password=YOUR_MYSQL_PASSWORD
```

### Lỗi: "Port 8080 already in use"

Sửa file `application.properties`:
```properties
server.port=8081
```

### Lỗi: "MQTT connection failed"

Kiểm tra:
1. MQTT broker có đang chạy?
2. IP `10.15.150.248` có đúng không?
3. Thử ping: `ping 10.15.150.248`

### Lỗi: "Java version error"

```powershell
# Cài JDK 21 từ https://adoptium.net/
# Sau đó set JAVA_HOME
```

## 📊 Xem dữ liệu trong MySQL

```sql
-- Đăng nhập MySQL
mysql -u root -p

-- Chọn database
USE iot_db;

-- Xem các bảng
SHOW TABLES;

-- Xem dữ liệu sensor
SELECT * FROM sensor_data ORDER BY received_at DESC LIMIT 10;

-- Xem trạng thái device
SELECT * FROM device_state ORDER BY timestamp DESC LIMIT 10;

-- Đếm số records
SELECT COUNT(*) FROM sensor_data;
SELECT COUNT(*) FROM device_state;
```

## 🔄 Test với ESP32

1. **Đảm bảo ESP32 đã upload code và chạy**
2. **Kiểm tra logs backend**, bạn sẽ thấy:
   ```
   MQTT message arrived [iot/demo/device/state] {...}
   Saved sensor data id=1
   Saved device state for device=demo, RGB=[255,0,0]
   ```

3. **Điều khiển từ backend**:
   - Mở `test-control-panel.html`
   - Click màu bất kỳ
   - ESP32 sẽ đổi màu LED!

## 📁 Cấu trúc quan trọng

```
backend_iot/
├── src/main/
│   ├── java/.../
│   │   ├── IotController.java      ← REST API endpoints
│   │   ├── MqttService.java        ← MQTT logic
│   │   ├── model/                  ← Database models
│   │   └── repository/             ← Database queries
│   └── resources/
│       └── application.properties  ← CẤU HÌNH CHÍNH
├── pom.xml                         ← Maven dependencies
├── run.bat                         ← Script chạy nhanh
├── test-control-panel.html        ← Test UI
└── SETUP.md                        ← Hướng dẫn chi tiết
```

## 🎯 API chính cần nhớ

| Method | Endpoint | Chức năng |
|--------|----------|-----------|
| GET | `/api/v1/info` | Thông tin hệ thống |
| POST | `/api/v1/device/led/rgb?r={}&g={}&b={}` | Đặt màu LED |
| POST | `/api/v1/device/led/off` | Tắt LED |
| GET | `/api/v1/device/state?deviceId=demo` | Xem trạng thái |
| GET | `/api/v1/sensor/recent` | Xem dữ liệu sensor |

## ✅ Checklist hoàn thành

- [ ] Cài đặt Java 21+
- [ ] Cài đặt Maven
- [ ] Cài đặt MySQL và tạo database `iot_db`
- [ ] Sửa password MySQL trong `application.properties`
- [ ] Build thành công: `mvn clean install`
- [ ] Chạy được server: `mvn spring-boot:run`
- [ ] Test API thành công: `http://localhost:8080/api/v1/info`
- [ ] Test với `test-control-panel.html`
- [ ] Kết nối được với ESP32
- [ ] Điều khiển LED thành công
- [ ] Thấy dữ liệu trong MySQL

## 💡 Tips

1. **Luôn xem logs** khi gặp lỗi - nó sẽ cho biết chính xác vấn đề
2. **Test từng bước** - MySQL → MQTT → REST API → ESP32
3. **Dùng test-control-panel.html** - dễ test hơn cURL
4. **Kiểm tra MySQL thường xuyên** - đảm bảo data được lưu

## 📞 Cần giúp thêm?

Xem các file hướng dẫn chi tiết:
- `SETUP.md` - Hướng dẫn setup đầy đủ
- `README_API.md` - Documentation về API
- `SUMMARY.md` - Tóm tắt những gì đã làm

---

**Chúc bạn thành công!** 🎉
