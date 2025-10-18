# ⚡ Quick Start Guide - 5 phút để chạy hệ thống

## 📋 Checklist trước khi bắt đầu

- [ ] PostgreSQL đã cài đặt và running
- [ ] MQTT Broker (Mosquitto hoặc Docker) đang chạy
- [ ] Node.js & npm đã cài đặt
- [ ] Flutter SDK đã cài đặt (nếu chạy mobile)
- [ ] Java 21 đã cài đặt

## 🚀 3 bước để chạy

### Bước 1: Setup Database (1 phút)

```powershell
# Mở psql hoặc pgAdmin
# Tạo database tên "IoT"
psql -U postgres -c "CREATE DATABASE \"IoT\";"
```

### Bước 2: Start MQTT Broker (30 giây)

**Option A: Mosquitto service**
```powershell
net start mosquitto
```

**Option B: Docker**
```powershell
docker run -d -p 1883:1883 --name mosquitto eclipse-mosquitto
```

### Bước 3: Start Applications (2 phút)

**Terminal 1 - Backend:**
```powershell
cd d:\iot-projects\baitapbuoi6
.\start-backend.ps1
```
Đợi đến khi thấy: `Started Btbuoi6Application`

**Terminal 2 - Frontend:**
```powershell
cd d:\iot-projects\baitapbuoi6
.\start-frontend.ps1
```
Browser tự động mở `http://localhost:3000`

**Done! 🎉**

## 🧪 Test nhanh (1 phút)

1. Trong Web, thêm thiết bị:
   - Tên: `Test Device`
   - Topic: `/test/topic`
   - Click **Tạo thiết bị**

2. Gửi lệnh: `{"hello":"world"}`

3. Mở MQTTX:
   - Connect: `localhost:1883`
   - Subscribe: `/test/topic`
   - Bạn sẽ thấy message `{"hello":"world"}`

**Success! ✅**

## 📱 Bonus: Chạy Mobile App (Optional)

```powershell
cd d:\iot-projects\baitapbuoi6\btbuoi6_flutter
flutter pub get
flutter run
```

---

## 🆘 Gặp lỗi?

### Backend không start?
```powershell
# Kiểm tra port 8080 có bị chiếm không
netstat -ano | findstr :8080

# Kiểm tra PostgreSQL
psql -U postgres -c "SELECT version();"
```

### Frontend lỗi npm?
```powershell
cd btbuoi6-frontend
rm -rf node_modules package-lock.json
npm install
```

### MQTT không connect?
```powershell
# Test port 1883
Test-NetConnection localhost -Port 1883
```

---

## 📚 Đọc thêm

- [README.md](README.md) - Hướng dẫn đầy đủ
- [TESTING.md](TESTING.md) - Quy trình test chi tiết
- [SUMMARY.md](SUMMARY.md) - Tổng kết dự án

**Happy coding! 💻**
