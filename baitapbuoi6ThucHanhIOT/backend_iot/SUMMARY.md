# 📝 TÓM TẮT CẬP NHẬT BACKEND IOT

## ✅ Đã hoàn thành

### 1. Tạo Model & Repository mới

#### Model `DeviceState.java`
- Lưu trạng thái LED RGB từ ESP32
- Fields: `id`, `deviceId`, `ledR`, `ledG`, `ledB`, `rawPayload`, `timestamp`

#### Repository `DeviceStateRepository.java`
- Methods để query trạng thái thiết bị
- `findFirstByDeviceIdOrderByTimestampDesc()` - Lấy trạng thái mới nhất
- `findByDeviceIdOrderByTimestampDesc()` - Lấy lịch sử
- `findTop100ByOrderByTimestampDesc()` - Lấy 100 records mới nhất

### 2. Tạo DTO (Data Transfer Objects)

#### `LedCommandRequest.java`
- DTO cho lệnh điều khiển LED
- Hỗ trợ `led_rgb` array và `off` boolean

#### `DeviceStateResponse.java`
- DTO cho response trạng thái thiết bị
- Format dữ liệu gọn gàng hơn cho frontend

### 3. Cập nhật MqttService.java

**Thêm chức năng:**
- Parse JSON từ MQTT message
- Lưu vào 2 bảng:
  - `sensor_data`: Lưu raw data
  - `device_state`: Lưu parsed RGB values
- Extract deviceId từ topic
- Error handling tốt hơn

**Dependencies mới:**
- `ObjectMapper` (Jackson) để parse JSON
- `DeviceStateRepository` để lưu trạng thái

### 4. Cập nhật IotController.java

**Thêm endpoints mới:**

#### Device Control:
- `POST /api/v1/device/led/rgb?r={}&g={}&b={}` - Set màu RGB
- `POST /api/v1/device/led/off` - Tắt LED
- `POST /api/v1/device/led` - Điều khiển qua JSON body

#### Device State:
- `GET /api/v1/device/state?deviceId={}` - Trạng thái hiện tại
- `GET /api/v1/device/state/history?deviceId={}&limit={}` - Lịch sử
- `GET /api/v1/device/states/recent` - Trạng thái gần đây

#### Sensor Data:
- `GET /api/v1/sensor/recent?limit={}` - Dữ liệu sensor gần đây
- `GET /api/v1/sensor/all` - Tất cả dữ liệu
- `GET /api/v1/sensor/{id}` - Dữ liệu theo ID

#### System:
- `GET /api/v1/info` - Thông tin hệ thống

### 5. Tạo WebConfig.java

- Cấu hình CORS để frontend có thể gọi API
- Allow all origins (có thể giới hạn trong production)

### 6. Cập nhật application.properties

**Thêm/Cập nhật:**
- `server.port=8080`
- `mqtt.broker=tcp://10.15.150.248:1883` - IP của broker ESP32
- `mqtt.subscribeTopic=iot/demo/#` - Subscribe tất cả topics trong namespace
- `mqtt.namespace=iot/demo` - Namespace cho topics
- `spring.datasource.url` - Thêm `createDatabaseIfNotExist=true`
- `spring.datasource.password=` - Để trống (tùy config MySQL)
- Logging configuration

### 7. Tạo files tài liệu và tools

#### `README_API.md`
- Hướng dẫn chi tiết về API
- Luồng hoạt động của hệ thống
- Examples với cURL
- Troubleshooting

#### `SETUP.md`
- Hướng dẫn setup từng bước
- Yêu cầu hệ thống
- Cách cài đặt và chạy
- Kiểm tra hoạt động
- Troubleshooting chi tiết

#### `database_setup.sql`
- Script tạo database và tables
- Indexes cho performance
- Sample queries

#### `run.bat`
- Script Windows để build và chạy
- Menu với nhiều options
- Dễ dùng cho người mới

#### `test-control-panel.html`
- Web UI để test API
- Điều khiển LED trực quan
- Color picker và preset colors
- Real-time status update
- Activity logs

#### `IoT_Backend_API.postman_collection.json`
- Collection đầy đủ các endpoints
- Organized by categories
- Variables cho dễ config

## 🗄️ Database Schema

### Bảng `sensor_data`
```sql
- id (BIGINT, AUTO_INCREMENT, PK)
- topic (VARCHAR(255))
- payload (TEXT)
- received_at (DATETIME)
```

### Bảng `device_state`
```sql
- id (BIGINT, AUTO_INCREMENT, PK)
- device_id (VARCHAR(100))
- led_r (INT)
- led_g (INT)
- led_b (INT)
- raw_payload (TEXT)
- timestamp (DATETIME)
```

## 🔄 Luồng dữ liệu

### ESP32 → Backend:
```
ESP32 publish: iot/demo/device/state
Payload: {"ts": "12345", "led_rgb": [255, 0, 0]}
↓
Backend MQTT Service nhận
↓
Lưu vào sensor_data (raw)
↓
Parse JSON và lưu vào device_state (RGB values)
```

### Client → ESP32:
```
Client gửi: POST /api/v1/device/led/rgb?r=0&g=255&b=0
↓
Backend publish: iot/demo/device/cmd
Payload: {"led_rgb": [0, 255, 0]}
↓
ESP32 nhận và thực hiện
↓
ESP32 publish lại trạng thái mới lên iot/demo/device/state
```

## 📋 Checklist Setup

- [ ] Cài Java 21+
- [ ] Cài Maven 3.8+
- [ ] Cài MySQL 8.0+
- [ ] Tạo database `iot_db`
- [ ] Cập nhật password MySQL trong `application.properties`
- [ ] Kiểm tra MQTT broker đang chạy (10.15.150.248:1883)
- [ ] Build project: `mvn clean install`
- [ ] Run project: `mvn spring-boot:run`
- [ ] Test API: Mở `http://localhost:8080/api/v1/info`
- [ ] Test với `test-control-panel.html`
- [ ] Upload code ESP32 và test end-to-end

## 🎯 Các API chính để test

```bash
# 1. Kiểm tra hệ thống
curl http://localhost:8080/api/v1/info

# 2. Điều khiển LED - Màu đỏ
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=255&g=0&b=0"

# 3. Điều khiển LED - Màu xanh
curl -X POST "http://localhost:8080/api/v1/device/led/rgb?r=0&g=255&b=0"

# 4. Tắt LED
curl -X POST "http://localhost:8080/api/v1/device/led/off"

# 5. Lấy trạng thái hiện tại
curl http://localhost:8080/api/v1/device/state?deviceId=demo

# 6. Lấy lịch sử trạng thái
curl "http://localhost:8080/api/v1/device/state/history?deviceId=demo&limit=10"

# 7. Lấy dữ liệu sensor
curl "http://localhost:8080/api/v1/sensor/recent?limit=20"
```

## 🔧 Cấu hình ESP32

ESP32 của bạn cần:
- Kết nối WiFi: `TDMU`
- MQTT Broker: `10.15.150.248:1883`
- Namespace: `iot/demo`
- Subscribe topic: `iot/demo/device/cmd`
- Publish topic: `iot/demo/device/state`

Code ESP32 hiện tại đã đúng config!

## 📂 Files đã tạo/sửa

### Java Files (Created):
1. `model/DeviceState.java`
2. `repository/DeviceStateRepository.java`
3. `dto/LedCommandRequest.java`
4. `dto/DeviceStateResponse.java`
5. `config/WebConfig.java`

### Java Files (Updated):
1. `MqttService.java` - Thêm logic parse và lưu device state
2. `IotController.java` - Thêm nhiều endpoints mới

### Config Files (Updated):
1. `application.properties` - Cập nhật config cho MQTT và MySQL

### Documentation Files (Created):
1. `README_API.md` - API documentation
2. `SETUP.md` - Setup guide
3. `database_setup.sql` - Database schema
4. `run.bat` - Build/run script
5. `test-control-panel.html` - Test UI
6. `IoT_Backend_API.postman_collection.json` - Postman collection
7. `SUMMARY.md` - File này

## ⚠️ Lưu ý quan trọng

1. **MySQL Password**: Đổi password trong `application.properties` nếu cần
2. **MQTT Broker IP**: Đảm bảo IP `10.15.150.248` đúng và accessible
3. **Port 8080**: Đổi nếu bị conflict với app khác
4. **CORS**: Hiện tại allow all origins, giới hạn trong production
5. **Database**: Auto-create enabled, nhưng nên tạo manual cho an toàn

## 🚀 Next Steps

1. **Test Backend độc lập**:
   - Start backend
   - Test với `test-control-panel.html`
   - Verify MySQL có data

2. **Test với ESP32**:
   - Upload code ESP32
   - Kiểm tra ESP32 publish được data
   - Backend nhận và lưu vào DB
   - Test điều khiển từ backend → ESP32

3. **Tích hợp Frontend**:
   - Integrate với `web-iot` hoặc `app_iot`
   - Update API endpoints trong frontend

4. **Production Ready**:
   - Add authentication
   - Add input validation
   - Add rate limiting
   - Proper error handling
   - Add unit tests

## 📞 Troubleshooting Quick Fix

| Lỗi | Giải pháp |
|-----|-----------|
| Cannot connect to MySQL | Kiểm tra MySQL running, password đúng |
| MQTT connection failed | Kiểm tra broker IP, ping thử |
| Port 8080 in use | Đổi port trong application.properties |
| Java version error | Cài JDK 21, set JAVA_HOME |
| Build failed | `mvn clean install -U` |
| No data from ESP32 | Kiểm tra ESP32 WiFi, MQTT topics |

---

**Hoàn thành:** Backend IoT với đầy đủ REST API, MQTT integration, và MySQL storage! 🎉
