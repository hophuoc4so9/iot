# 🧪 Hướng dẫn Test hệ thống IoT

## 📝 Checklist trước khi test

- [ ] PostgreSQL đang chạy
- [ ] Database "IoT" đã được tạo
- [ ] MQTT Broker (Mosquitto) đang chạy trên port 1883
- [ ] MQTTX đã được cài đặt

## 🚀 Quy trình test đầy đủ

### 1. Khởi động Backend

```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6
.\mvnw spring-boot:run
```

**Kiểm tra:** Console hiển thị:
```
Started Btbuoi6Application in X.XXX seconds
```

**Test API trực tiếp:**
```powershell
curl http://localhost:8080/devices
# Kết quả: [] (mảng rỗng nếu chưa có device)
```

### 2. Khởi động ReactJS Frontend

Mở terminal mới:
```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6-frontend
npm start
```

Browser sẽ tự động mở `http://localhost:3000`

### 3. Setup MQTTX

1. **Mở MQTTX**
2. **Tạo Connection:**
   - Name: `Local IoT Broker`
   - Host: `127.0.0.1`
   - Port: `1883`
   - Client ID: `mqttx_client`
   - Click **Connect**

3. **Subscribe Topics:**
   - Click **+ New Subscription**
   - Topic: `/sensor/temp` → Subscribe
   - Topic: `/sensor/humidity` → Subscribe
   - QoS: 0

### 4. Test Scenario 1: Tạo thiết bị và gửi lệnh

**Bước 1: Tạo thiết bị từ Web**
1. Scroll xuống phần "➕ Thêm thiết bị mới"
2. Nhập:
   - Tên thiết bị: `Temperature Sensor`
   - Topic MQTT: `/sensor/temp`
3. Click **Tạo thiết bị**

**Bước 2: Gửi lệnh điều khiển**
1. Trong card `Temperature Sensor`
2. Nhập lệnh: `{"command":"set_temp","value":25}`
3. Click **Gửi lệnh**

**Bước 3: Kiểm tra MQTTX**
- Trong MQTTX topic `/sensor/temp` sẽ hiển thị:
  ```json
  {"command":"set_temp","value":25}
  ```

### 5. Test Scenario 2: Nhận dữ liệu từ thiết bị

**Bước 1: Publish từ MQTTX**
1. Trong MQTTX, chọn topic `/sensor/temp`
2. Nhập payload:
   ```json
   {"temperature":28.5,"status":"normal"}
   ```
3. Click **Publish**

**Bước 2: Kiểm tra Backend**
- Backend console sẽ log:
  ```
  Received MQTT message: {"temperature":28.5,"status":"normal"}
  ```

**Bước 3: Xem Telemetry trong Web**
1. Click **Xem dữ liệu** ở `Temperature Sensor`
2. Dialog hiển thị message vừa nhận với timestamp

### 6. Test Scenario 3: Multiple Devices

**Tạo thêm 2 thiết bị:**

Device 2:
- Tên: `Humidity Sensor`
- Topic: `/sensor/humidity`

Device 3:
- Tên: `Light Control`
- Topic: `/device/light`

**Test parallel commands:**
1. Gửi lệnh cho `Humidity Sensor`: `{"humidity":65}`
2. Gửi lệnh cho `Light Control`: `{"action":"on","brightness":80}`
3. Kiểm tra cả 2 message trong MQTTX

### 7. Test với Flutter Mobile App

**Bước 1: Chạy Flutter**
```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6_flutter
flutter run
```

**Bước 2: Kiểm tra các tính năng**
- [ ] App hiển thị danh sách 3 thiết bị đã tạo
- [ ] Pull down để refresh → Danh sách update
- [ ] Click vào một thiết bị
- [ ] Nhập lệnh: `{"mobile":"test"}`
- [ ] Click **Gửi lệnh**
- [ ] Kiểm tra MQTTX nhận được message
- [ ] Click **Xem dữ liệu** → Hiển thị telemetry

### 8. Test Database Persistence

**Bước 1: Stop Backend**
```
Ctrl + C trong terminal backend
```

**Bước 2: Restart Backend**
```powershell
.\mvnw spring-boot:run
```

**Bước 3: Refresh Web/App**
- Tất cả 3 thiết bị vẫn hiển thị (đã lưu trong DB)
- Telemetry data vẫn còn

### 9. Test Error Handling

**Scenario A: Backend bị tắt**
1. Stop backend (Ctrl+C)
2. Trong Web, thử tạo thiết bị mới
3. Kết quả: Alert "Lỗi khi tạo thiết bị!"

**Scenario B: Dữ liệu không hợp lệ**
1. Tạo device với tên rỗng
2. Kết quả: Alert "Vui lòng điền đầy đủ thông tin!"

**Scenario C: MQTT Broker bị tắt**
1. Stop Mosquitto: `net stop mosquitto`
2. Thử gửi lệnh
3. Backend sẽ tự động reconnect khi broker bật lại
4. Start lại: `net start mosquitto`

## 📊 Kết quả mong đợi

| Chức năng | Web | Mobile | Status |
|-----------|-----|--------|--------|
| Xem danh sách thiết bị | ✅ | ✅ | Pass |
| Tạo thiết bị mới | ✅ | ✅ | Pass |
| Gửi lệnh MQTT | ✅ | ✅ | Pass |
| Nhận message MQTT | ✅ | ✅ | Pass |
| Xem telemetry | ✅ | ✅ | Pass |
| Persistence (DB) | ✅ | ✅ | Pass |
| Error handling | ✅ | ✅ | Pass |

## 🔧 Debug Tips

### Backend logs không hiển thị MQTT messages
```
Kiểm tra:
1. Topic trong device có đúng không
2. QoS level có match không
3. Xem MqttConfig.java → inbound adapter có subscribe đúng topic không
```

### Web không kết nối được Backend
```
1. Kiểm tra Network tab trong F12 DevTools
2. Xem CORS errors
3. Verify backend đang chạy: curl http://localhost:8080/devices
```

### Flutter không connect được
```
1. Kiểm tra _baseUrl trong main.dart
2. Với emulator: http://10.0.2.2:8080
3. Với device: http://<YOUR_IP>:8080
4. Check Flutter logs: flutter logs
```

## 📸 Screenshots checklist

Chụp màn hình để documentation:
- [ ] MQTTX connected với broker
- [ ] Web: Danh sách 3 devices
- [ ] Web: Dialog telemetry data
- [ ] Mobile: Home screen với devices
- [ ] Mobile: Dialog telemetry
- [ ] Backend console logs
- [ ] PostgreSQL pgAdmin: Bảng devices & telemetry

## ✅ Test Completion

Sau khi hoàn thành tất cả test cases trên:
- [ ] Backend stable, không crash
- [ ] Web responsive, UI/UX tốt
- [ ] Mobile mượt mà, không lag
- [ ] MQTT messages được deliver chính xác
- [ ] Database lưu trữ đúng
- [ ] Error handling hoạt động

---

**Happy Testing! 🎉**
