# Hướng dẫn Setup Backend IoT

## 📋 Tổng quan

Backend này được xây dựng bằng Spring Boot để:
- Nhận dữ liệu từ ESP32 qua MQTT
- Lưu trữ dữ liệu vào MySQL
- Cung cấp REST API để điều khiển và giám sát thiết bị

## 🔧 Yêu cầu hệ thống

### Phần mềm cần cài đặt:

1. **Java Development Kit (JDK) 21+**
   - Download: https://adoptium.net/
   - Kiểm tra: `java -version`

2. **Apache Maven 3.8+**
   - Download: https://maven.apache.org/download.cgi
   - Kiểm tra: `mvn -version`

3. **MySQL Server 8.0+**
   - Download: https://dev.mysql.com/downloads/mysql/
   - Kiểm tra: `mysql --version`

4. **MQTT Broker** (đã có sẵn tại 10.15.150.248:1883)
   - Hoặc cài Mosquitto local: https://mosquitto.org/download/

## 📦 Cài đặt

### Bước 1: Cấu hình MySQL

1. Khởi động MySQL server
2. Tạo database:

```sql
CREATE DATABASE iot_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

Hoặc chạy file SQL:
```bash
mysql -u root -p < database_setup.sql
```

### Bước 2: Cấu hình ứng dụng

Chỉnh sửa `src/main/resources/application.properties`:

```properties
# MySQL - Đổi password của bạn
spring.datasource.password=your_mysql_password

# MQTT Broker - IP của broker
mqtt.broker=tcp://10.15.150.248:1883
```

### Bước 3: Build và chạy

#### Cách 1: Sử dụng script (Windows)
```bash
run.bat
# Chọn option 3 (Build and Run)
```

#### Cách 2: Sử dụng Maven
```bash
# Build
mvn clean install

# Run
mvn spring-boot:run
```

#### Cách 3: Chạy JAR
```bash
# Build JAR
mvn clean package

# Run JAR
java -jar target/backend_iot-0.0.1-SNAPSHOT.jar
```

## ✅ Kiểm tra hoạt động

### 1. Kiểm tra server đã chạy

Mở browser và truy cập:
```
http://localhost:8080/api/v1/info
```

Kết quả mong đợi:
```json
{
  "namespace": "iot/demo",
  "topicDeviceCmd": "iot/demo/device/cmd",
  "topicDeviceState": "iot/demo/device/state",
  "totalSensorRecords": 0,
  "totalDeviceStateRecords": 0
}
```

### 2. Kiểm tra kết nối MySQL

Kiểm tra logs, tìm dòng:
```
Hibernate: create table...
```

Hoặc kiểm tra database:
```sql
USE iot_db;
SHOW TABLES;
-- Kết quả: sensor_data, device_state
```

### 3. Kiểm tra kết nối MQTT

Kiểm tra logs, tìm dòng:
```
Connected to MQTT broker tcp://10.15.150.248:1883
Subscribed to topic iot/demo/#
```

## 🧪 Test API

### Cách 1: Sử dụng Test Control Panel (HTML)

1. Mở file `test-control-panel.html` trong browser
2. Click các nút màu để điều khiển LED
3. Xem trạng thái và logs

### Cách 2: Sử dụng cURL

```bash
# Đặt LED màu đỏ
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=255&g=0&b=0"

# Đặt LED màu xanh lá
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=0&g=255&b=0"

# Tắt LED
curl -X POST "http://localhost:8080/api/v1/device/led/off"

# Lấy trạng thái
curl "http://localhost:8080/api/v1/device/state?deviceId=demo"
```

### Cách 3: Sử dụng Postman

1. Import file `IoT_Backend_API.postman_collection.json` vào Postman
2. Test các endpoints

## 🔄 Luồng hoạt động

```
ESP32 --[MQTT]--> Backend --[MySQL]--> Database
  ^                  |
  |                  v
  +----[MQTT]---- REST API <--[HTTP]-- Web/Mobile Client
```

### Chi tiết:

1. **ESP32 publish trạng thái** lên topic `iot/demo/device/state`:
   ```json
   {
     "ts": "12345",
     "led_rgb": [255, 0, 0]
   }
   ```

2. **Backend nhận và lưu vào MySQL**:
   - Bảng `sensor_data`: Lưu raw message
   - Bảng `device_state`: Lưu parsed RGB values

3. **Client gửi lệnh điều khiển** qua REST API:
   ```
   POST /api/v1/device/led/rgb?r=0&g=255&b=0
   ```

4. **Backend publish lệnh** lên topic `iot/demo/device/cmd`:
   ```json
   {
     "led_rgb": [0, 255, 0]
   }
   ```

5. **ESP32 nhận lệnh** và thực hiện, sau đó publish lại trạng thái mới

## 📊 Cấu trúc Database

### Bảng `sensor_data`
```sql
CREATE TABLE sensor_data (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    topic VARCHAR(255) NOT NULL,
    payload TEXT,
    received_at DATETIME NOT NULL
);
```

### Bảng `device_state`
```sql
CREATE TABLE device_state (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(100) NOT NULL,
    led_r INT NOT NULL,
    led_g INT NOT NULL,
    led_b INT NOT NULL,
    raw_payload TEXT,
    timestamp DATETIME NOT NULL
);
```

## 🐛 Troubleshooting

### Lỗi: "Cannot connect to MySQL"

**Nguyên nhân**: MySQL chưa chạy hoặc sai password

**Giải pháp**:
1. Kiểm tra MySQL đang chạy: `mysql -u root -p`
2. Kiểm tra password trong `application.properties`
3. Tạo database: `CREATE DATABASE iot_db;`

### Lỗi: "MQTT connection failed"

**Nguyên nhân**: Không kết nối được MQTT broker

**Giải pháp**:
1. Ping broker: `ping 10.15.150.248`
2. Kiểm tra broker đang chạy
3. Thử đổi IP trong `application.properties`

### Lỗi: "Port 8080 already in use"

**Nguyên nhân**: Port 8080 đã được sử dụng

**Giải pháp**:
Đổi port trong `application.properties`:
```properties
server.port=8081
```

### Lỗi: "Java version mismatch"

**Nguyên nhân**: JDK version không đúng

**Giải pháp**:
1. Kiểm tra: `java -version`
2. Cài JDK 21: https://adoptium.net/
3. Set JAVA_HOME

### Không nhận được dữ liệu từ ESP32

**Kiểm tra**:
1. ESP32 đã kết nối WiFi chưa?
2. ESP32 publish đúng topic chưa? (`iot/demo/device/state`)
3. Backend subscribe đúng topic chưa? (`iot/demo/#`)
4. Kiểm tra logs backend

## 📁 Cấu trúc Project

```
backend_iot/
├── src/
│   ├── main/
│   │   ├── java/backend_iot/backend_iot/
│   │   │   ├── BackendIotApplication.java    # Main application
│   │   │   ├── IotController.java            # REST API endpoints
│   │   │   ├── MqttService.java              # MQTT client service
│   │   │   ├── config/
│   │   │   │   └── WebConfig.java            # CORS configuration
│   │   │   ├── dto/
│   │   │   │   ├── LedCommandRequest.java    # Command DTO
│   │   │   │   └── DeviceStateResponse.java  # Response DTO
│   │   │   ├── model/
│   │   │   │   ├── SensorData.java           # Sensor data entity
│   │   │   │   └── DeviceState.java          # Device state entity
│   │   │   └── repository/
│   │   │       ├── SensorDataRepository.java
│   │   │       └── DeviceStateRepository.java
│   │   └── resources/
│   │       └── application.properties         # Configuration
├── pom.xml                                    # Maven dependencies
├── README_API.md                              # API documentation
├── database_setup.sql                         # Database schema
├── run.bat                                    # Build/run script
├── test-control-panel.html                   # Test UI
└── IoT_Backend_API.postman_collection.json   # Postman collection
```

## 🚀 API Endpoints Summary

### Device Control
- `POST /api/v1/device/led/rgb?r={r}&g={g}&b={b}` - Set LED color
- `POST /api/v1/device/led/off` - Turn off LED
- `POST /api/v1/device/led` - Control LED (JSON body)

### Device State
- `GET /api/v1/device/state?deviceId={id}` - Get current state
- `GET /api/v1/device/state/history?deviceId={id}&limit={n}` - Get history
- `GET /api/v1/device/states/recent` - Get recent states

### Sensor Data
- `GET /api/v1/sensor/recent?limit={n}` - Get recent sensor data
- `GET /api/v1/sensor/all` - Get all sensor data
- `GET /api/v1/sensor/{id}` - Get sensor data by ID

### System
- `GET /api/v1/info` - Get system information
- `POST /api/v1/publish?topic={topic}` - Publish to MQTT

## 📞 Support

Nếu gặp vấn đề:
1. Kiểm tra logs trong console
2. Kiểm tra MySQL logs
3. Test từng component riêng lẻ (MySQL, MQTT, REST API)

## 🎓 Next Steps

1. Test backend với ESP32 thật
2. Tích hợp với frontend (web-iot hoặc app_iot)
3. Thêm authentication/authorization
4. Thêm WebSocket cho real-time updates
5. Deploy lên cloud (AWS, Azure, Heroku)
