# Backend IoT - REST API & MQTT Integration

Backend Spring Boot để nhận dữ liệu từ ESP32 qua MQTT và cung cấp REST API.

## Yêu cầu

- Java 21+
- MySQL 8.0+
- Maven 3.8+
- MQTT Broker (đang sử dụng broker tại 10.15.150.248:1883)

## Cấu hình

### 1. Cấu hình MySQL

Tạo database MySQL (hoặc để auto-create):

```sql
CREATE DATABASE iot_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

### 2. Cấu hình application.properties

Chỉnh sửa file `src/main/resources/application.properties`:

```properties
# MySQL
spring.datasource.url=jdbc:mysql://localhost:3306/iot_db?useSSL=false&serverTimezone=UTC&createDatabaseIfNotExist=true
spring.datasource.username=root
spring.datasource.password=your_password

# MQTT
mqtt.broker=tcp://10.15.150.248:1883
mqtt.namespace=iot/demo
```

## Chạy ứng dụng

### Sử dụng Maven

```bash
mvn clean install
mvn spring-boot:run
```

### Sử dụng JAR

```bash
mvn clean package
java -jar target/backend_iot-0.0.1-SNAPSHOT.jar
```

Server sẽ chạy tại: `http://localhost:8080`

## Cấu trúc Database

### Bảng `sensor_data`
Lưu tất cả dữ liệu thô từ MQTT:
- `id` (BIGINT, PK)
- `topic` (VARCHAR)
- `payload` (TEXT)
- `received_at` (DATETIME)

### Bảng `device_state`
Lưu trạng thái LED RGB của ESP32:
- `id` (BIGINT, PK)
- `device_id` (VARCHAR)
- `led_r` (INT)
- `led_g` (INT)
- `led_b` (INT)
- `raw_payload` (TEXT)
- `timestamp` (DATETIME)

## REST API Endpoints

### Thông tin hệ thống

**GET** `/api/v1/info`
```json
{
  "namespace": "iot/demo",
  "topicDeviceCmd": "iot/demo/device/cmd",
  "topicDeviceState": "iot/demo/device/state",
  "totalSensorRecords": 123,
  "totalDeviceStateRecords": 45
}
```

### Điều khiển LED

**POST** `/api/v1/device/led/rgb?r={0-255}&g={0-255}&b={0-255}`

Ví dụ: Đặt LED màu đỏ
```bash
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=255&g=0&b=0"
```

**POST** `/api/v1/device/led/off`

Tắt LED:
```bash
curl -X POST "http://localhost:8080/api/v1/device/led/off"
```

**POST** `/api/v1/device/led`

Body:
```json
{
  "led_rgb": [255, 128, 0]
}
```

hoặc:
```json
{
  "off": true
}
```

### Trạng thái thiết bị

**GET** `/api/v1/device/state?deviceId=demo`

Lấy trạng thái LED hiện tại:
```json
{
  "id": 1,
  "deviceId": "demo",
  "ledRgb": [255, 0, 0],
  "timestamp": "2025-10-11T10:30:00"
}
```

**GET** `/api/v1/device/state/history?deviceId=demo&limit=50`

Lấy lịch sử trạng thái LED.

**GET** `/api/v1/device/states/recent`

Lấy 100 trạng thái gần nhất.

### Dữ liệu sensor

**GET** `/api/v1/sensor/recent?limit=100`

Lấy dữ liệu sensor gần nhất.

**GET** `/api/v1/sensor/all`

Lấy tất cả dữ liệu sensor.

**GET** `/api/v1/sensor/{id}`

Lấy dữ liệu sensor theo ID.

**POST** `/api/v1/sensor?topic=custom/topic`

Body: Raw payload string

### MQTT Publish

**POST** `/api/v1/publish?topic=iot/demo/device/cmd`

Body: JSON string để publish lên MQTT broker.

## MQTT Topics

- **Subscribe**: `iot/demo/#` (nhận tất cả message từ namespace)
- **Device State**: `iot/demo/device/state` (ESP32 publish trạng thái)
- **Device Command**: `iot/demo/device/cmd` (Backend publish lệnh điều khiển)

## Luồng hoạt động

1. **ESP32 → Backend**: ESP32 publish trạng thái LED lên topic `iot/demo/device/state`
   ```json
   {
     "ts": "12345",
     "led_rgb": [255, 0, 0]
   }
   ```

2. **Backend nhận và lưu**: 
   - Lưu vào bảng `sensor_data` (dữ liệu thô)
   - Parse JSON và lưu vào bảng `device_state` (trạng thái LED)

3. **Frontend/Client → Backend**: Gọi REST API để điều khiển LED
   ```
   POST /api/v1/device/led/rgb?r=0&g=255&b=0
   ```

4. **Backend → ESP32**: Backend publish lệnh lên topic `iot/demo/device/cmd`
   ```json
   {
     "led_rgb": [0, 255, 0]
   }
   ```

5. **ESP32 nhận lệnh**: ESP32 thực hiện và publish lại trạng thái mới

## Testing với cURL

```bash
# Lấy thông tin hệ thống
curl http://localhost:8080/api/v1/info

# Đặt LED màu xanh lá
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=0&g=255&b=0"

# Đặt LED màu xanh dương
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=0&g=0&b=255"

# Tắt LED
curl -X POST "http://localhost:8080/api/v1/device/led/off"

# Lấy trạng thái hiện tại
curl http://localhost:8080/api/v1/device/state?deviceId=demo

# Lấy lịch sử 10 trạng thái gần nhất
curl "http://localhost:8080/api/v1/device/state/history?deviceId=demo&limit=10"

# Lấy dữ liệu sensor
curl "http://localhost:8080/api/v1/sensor/recent?limit=20"
```

## Troubleshooting

### Không kết nối được MySQL

- Kiểm tra MySQL đã chạy chưa
- Kiểm tra username/password trong `application.properties`
- Kiểm tra firewall

### Không kết nối được MQTT Broker

- Kiểm tra IP broker: `10.15.150.248:1883`
- Ping thử: `ping 10.15.150.248`
- Test bằng MQTT client: `mosquitto_pub -h 10.15.150.248 -t test -m "hello"`

### Port 8080 đã được sử dụng

Thay đổi port trong `application.properties`:
```properties
server.port=8081
```

## Logs

Xem logs để debug:
```bash
tail -f logs/spring.log
```

Hoặc xem console output khi chạy ứng dụng.
