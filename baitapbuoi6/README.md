# 📡 IoT Device Dashboard - Hệ thống quản lý thiết bị IoT

Hệ thống hoàn chỉnh bao gồm **Java Spring Boot Backend**, **ReactJS Frontend**, và **Flutter Mobile App** sử dụng Eclipse Paho MQTT Client để kết nối với MQTT Broker.

## 🎯 Mục tiêu

Tạo và quản lý ứng dụng IoT với các tính năng:
- ✅ Hiển thị danh sách thiết bị từ database
- ✅ Đăng ký thiết bị mới
- ✅ Điều khiển thiết bị qua MQTT
- ✅ Lưu trữ và xem dữ liệu telemetry

## 🏗️ Kiến trúc hệ thống

```
┌─────────────────┐         ┌─────────────────┐
│  ReactJS Web    │────────▶│  Spring Boot    │
│   Frontend      │         │    Backend      │
└─────────────────┘         │  (Port 8080)    │
                            └────────┬────────┘
┌─────────────────┐                 │
│  Flutter Mobile │────────────────▶│
│      App        │                 │
└─────────────────┘                 │
                                    ▼
                          ┌─────────────────┐
                          │  MQTT Broker    │
                          │  (Port 1883)    │
                          └─────────────────┘
                                    │
                                    ▼
                          ┌─────────────────┐
                          │   PostgreSQL    │
                          │  Database (IoT) │
                          └─────────────────┘
```

## 📋 Yêu cầu hệ thống

### Backend (Java Spring Boot)
- ☕ Java 21 hoặc cao hơn
- 📦 Maven
- 🐘 PostgreSQL 12+
- 🔌 MQTT Broker (Mosquitto/EMQX)

### Frontend (ReactJS)
- 📗 Node.js 16+ và npm
- 🌐 Browser hiện đại (Chrome, Firefox, Edge)

### Mobile (Flutter)
- 📱 Flutter SDK 3.0+
- 📲 Android Studio / VS Code với Flutter extension
- 🤖 Android Emulator hoặc thiết bị thật

## 🚀 Hướng dẫn cài đặt

### 1️⃣ Cài đặt PostgreSQL Database

```powershell
# Tạo database
psql -U postgres
CREATE DATABASE "IoT";
\q
```

Hoặc sử dụng pgAdmin để tạo database có tên `IoT`.

### 2️⃣ Cài đặt MQTT Broker

**Cách 1: Sử dụng Mosquitto**
```powershell
# Download từ: https://mosquitto.org/download/
# Sau khi cài đặt, start service:
net start mosquitto
```

**Cách 2: Sử dụng Docker**
```powershell
docker run -d -p 1883:1883 --name mosquitto eclipse-mosquitto
```

### 3️⃣ Chạy Backend (Spring Boot)

```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6

# Cập nhật application.properties nếu cần thay đổi password PostgreSQL
# Mặc định: username=postgres, password=1

# Build và chạy
.\mvnw clean install
.\mvnw spring-boot:run
```

Backend sẽ chạy tại: `http://localhost:8080`

**Test API:**
```powershell
# Lấy danh sách thiết bị
curl http://localhost:8080/devices

# Tạo thiết bị mới
curl -X POST http://localhost:8080/devices -H "Content-Type: application/json" -d "{\"name\":\"Temperature Sensor\",\"topic\":\"/sensor/temp\"}"
```

### 4️⃣ Chạy Frontend (ReactJS)

```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6-frontend

# Cài đặt dependencies
npm install

# Chạy development server
npm start
```

Frontend sẽ mở tại: `http://localhost:3000`

### 5️⃣ Chạy Mobile App (Flutter)

```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6_flutter

# Lấy dependencies
flutter pub get

# Kiểm tra devices
flutter devices

# Chạy trên Android Emulator
flutter run

# Hoặc chạy trên thiết bị cụ thể
flutter run -d <device_id>
```

**Lưu ý cho Android Emulator:**
- URL backend đã được cấu hình: `http://10.0.2.2:8080`
- `10.0.2.2` là địa chỉ localhost của máy host từ góc độ emulator

**Lưu ý cho thiết bị thật:**
- Thay đổi `_baseUrl` trong `main.dart` thành IP thật của máy:
```dart
final _baseUrl = 'http://192.168.1.100:8080'; // Thay bằng IP của bạn
```

## 🧪 Hướng dẫn test hệ thống

### Bước 1: Cài đặt MQTTX (Client để test MQTT)

Download MQTTX từ: https://mqttx.app/downloads

### Bước 2: Kết nối MQTTX với Broker

1. Mở MQTTX
2. Tạo connection mới:
   - **Name:** Local Broker
   - **Host:** `localhost` hoặc `127.0.0.1`
   - **Port:** `1883`
   - **Protocol:** `mqtt://`
3. Click **Connect**

### Bước 3: Subscribe topic trong MQTTX

1. Click **+ New Subscription**
2. Nhập topic: `/sensor/temp`
3. QoS: `0`
4. Click **Subscribe**

### Bước 4: Test flow hoàn chỉnh

**Từ Web/Mobile App:**
1. Tạo thiết bị mới:
   - Tên: `Temperature Sensor`
   - Topic: `/sensor/temp`
2. Nhập lệnh: `{"data":25,"unit":"celsius"}`
3. Click **Gửi lệnh**

**Trong MQTTX:**
- Bạn sẽ thấy message hiển thị: `{"data":25,"unit":"celsius"}`

**Publish từ MQTTX:**
1. Trong MQTTX, nhập topic: `/sensor/temp`
2. Nhập message: `{"temperature":30,"humidity":65}`
3. Click **Publish**

**Trong App:**
- Click **Xem dữ liệu** để xem telemetry đã lưu

## 📁 Cấu trúc project

```
baitapbuoi6/
├── btbuoi6/                          # Backend Java Spring Boot
│   ├── src/main/java/btbuoi6/btbuoi6/
│   │   ├── config/
│   │   │   ├── MqttConfig.java       # Cấu hình MQTT
│   │   │   └── WebConfig.java        # Cấu hình CORS
│   │   ├── controller/
│   │   │   ├── DeviceController.java
│   │   │   └── TelemetryController.java
│   │   ├── model/
│   │   │   ├── Device.java
│   │   │   └── Telemetry.java
│   │   ├── repository/
│   │   │   ├── DeviceRepository.java
│   │   │   └── TelemetryRepository.java
│   │   ├── service/
│   │   │   └── MqttPublisherService.java
│   │   └── Btbuoi6Application.java
│   ├── src/main/resources/
│   │   └── application.properties
│   └── pom.xml
│
├── btbuoi6-frontend/                 # Frontend ReactJS
│   ├── public/
│   │   └── index.html
│   ├── src/
│   │   ├── App.js                    # Main component
│   │   ├── App.css
│   │   └── index.js
│   └── package.json
│
└── btbuoi6_flutter/                  # Mobile Flutter
    ├── lib/
    │   └── main.dart                 # Flutter app
    └── pubspec.yaml
```

## 🔌 API Endpoints

### Devices
- `GET /devices` - Lấy danh sách tất cả thiết bị
- `POST /devices` - Tạo thiết bị mới
  ```json
  {
    "name": "Temperature Sensor",
    "topic": "/sensor/temp"
  }
  ```
- `POST /devices/{id}/control` - Gửi lệnh điều khiển
  - Body: Plain text (ví dụ: `{"data":20}`)

### Telemetry
- `GET /telemetry/{deviceId}` - Lấy telemetry theo thiết bị
- `GET /telemetry` - Lấy tất cả telemetry

## 🛠️ Công nghệ sử dụng

### Backend
- **Spring Boot 3.5.6** - Framework Java
- **Spring Integration MQTT** - MQTT integration
- **Eclipse Paho 1.2.5** - MQTT client
- **Spring Data JPA** - ORM
- **PostgreSQL** - Database
- **Jackson** - JSON processing

### Frontend
- **React 18.2** - UI library
- **Material-UI (MUI) 5** - Component library
- **Axios** - HTTP client
- **@mui/icons-material** - Icons

### Mobile
- **Flutter 3+** - Mobile framework
- **http 1.2** - HTTP client
- **Material Design** - UI components

## 🐛 Xử lý lỗi thường gặp

### Backend không connect được PostgreSQL
```
Kiểm tra:
- PostgreSQL service đã chạy chưa
- Database "IoT" đã được tạo chưa
- Username/password trong application.properties đúng chưa
```

### Backend không connect được MQTT Broker
```
Kiểm tra:
- Mosquitto/MQTT Broker đã chạy chưa (port 1883)
- Firewall có block port 1883 không
- Đổi brokerUrl trong MqttConfig.java nếu cần
```

### Frontend không gọi được API
```
Kiểm tra:
- Backend đã chạy tại port 8080 chưa
- CORS đã được cấu hình trong WebConfig.java
- Browser console có lỗi gì không
```

### Flutter không connect được Backend
```
- Với Emulator: Dùng http://10.0.2.2:8080
- Với thiết bị thật: Dùng IP thật của máy (VD: http://192.168.1.100:8080)
- Đảm bảo máy và thiết bị cùng mạng WiFi
```

## 📝 Ghi chú

- Mặc định database sẽ tự động tạo bảng (Hibernate ddl-auto=update)
- MQTT QoS level: 0 hoặc 1
- Retained messages: Có thể cấu hình trong publish
- CORS đã được bật cho `http://localhost:3000`

## 👨‍💻 Phát triển thêm

Các tính năng có thể mở rộng:
- 🔐 Authentication & Authorization (JWT)
- 📊 Dashboard với charts (Chart.js/Recharts)
- 🔔 Real-time notifications (WebSocket)
- 📱 Push notifications cho mobile
- 🔄 Auto-refresh telemetry data
- 🎨 Dark mode theme
- 🌍 Multi-language support
- 📈 Analytics và reporting

## 📞 Hỗ trợ

Nếu gặp vấn đề, vui lòng:
1. Kiểm tra logs của backend trong console
2. Kiểm tra Network tab trong Browser DevTools
3. Xem Flutter logs với `flutter logs`
4. Đảm bảo tất cả services (PostgreSQL, MQTT, Backend) đang chạy

---

**Chúc bạn học tốt! 🎓📚**
