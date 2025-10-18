

Chạy ứng dụng

cd backend_iot

# Chạy Mosquitto MQTT Broker
docker-compose up -d


.\mvnw.cmd  clean install


.\mvnw.cmd  spring-boot:run

Backend sẽ chạy tại: `http://localhost:8080`

MQTT Broker: `tcp://localhost:1883`




cd esp32
pio run --target upload
pio device monitor 


### 3. Web Dashboard


cd web-iot

# Cài đặt dependencies
npm install

# Cập nhật .env với backend URL
echo "VITE_API_BASE=http://YOUR_BACKEND_IP:8080" > .env

# Chạy development server
npm run dev
```

Web dashboard sẽ chạy tại: `http://localhost:5173`

### 4. Flutter App

#### Yêu cầu:
- Flutter SDK 3.0+
- Android Studio / Xcode (cho build mobile)

#### Cài đặt:

```bash
cd app_iot

# Cài đặt dependencies
flutter pub get


flutter run
```

## 📡 MQTT Topics



## 🌐 REST API Endpoints

### GET `/api/control/status/latest`
Lấy telemetry mới nhất

### GET `/api/telemetry/recent`
Lấy 100 bản ghi telemetry gần nhất

### POST `/api/control/mode?mode=AUTO`
Đổi mode (AUTO hoặc MANUAL)

### POST `/api/control/motor?cmd=FORWARD`
Điều khiển motor (FORWARD, BACKWARD, STOP)

### POST `/api/control/motor?cmd=DUTY&duty=512`
Đặt duty cycle (0-1023)

## 🔌 Kết nối phần cứng ESP32

### Chân GPIO:

```
DS18B20 Temperature Sensor:
- Data: GPIO 4
- VCC: 3.3V
- GND: GND
- Pull-up resistor: 4.7kΩ giữa Data và VCC

Float Switch HIGH (bể đầy):
- GPIO 15 (INPUT_PULLUP)
- LOW = water present, HIGH = no water

Float Switch LOW (mức tối thiểu):
- GPIO 16 (INPUT_PULLUP)
- LOW = water present, HIGH = no water

Motor Driver:
- PWM: GPIO 5 (tốc độ)
- RPWM: GPIO 6 (forward)
- LPWM: GPIO 7 (backward)
```

## 🚀 Luồng hoạt động

1. **ESP32** đọc cảm biến nhiệt độ và float switches mỗi 2 giây
2. **ESP32** publish dữ liệu lên MQTT topic `esp32/telemetry`
3. **Backend** nhận dữ liệu từ MQTT, lưu vào MySQL database
4. **Web/App** poll REST API `/api/control/status/latest` mỗi 2 giây để cập nhật UI
5. User điều khiển từ **Web/App** → Backend publish command lên MQTT → ESP32 thực thi

### AUTO Mode:
```
if (floatLow == false) → START MOTOR FORWARD
if (floatHigh == true) → RUN MOTOR BACKWARD 2s → STOP
```




## 📄 License

MIT License


