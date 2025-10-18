# IoT Projects - ESP32-S3 Development

Dự án thực hành IoT sử dụng ESP-IDF framework với ESP32-S3 và Visual Studio Code.

## 📋 Mục lục

- [Giới thiệu](#giới-thiệu)
- [Công nghệ sử dụng](#công-nghệ-sử-dụng)
- [Cấu trúc dự án](#cấu-trúc-dự-án)
- [Các Project trong repo](#các-project-trong-repo)
- [Yêu cầu hệ thống](#yêu-cầu-hệ-ống)
- [Hướng dẫn cài đặt](#hướng-dẫn-cài-đặt)
- [Đóng góp](#đóng-góp)

## 🎯 Giới thiệu

Repository này chứa các bài tập và project thực hành IoT, bao gồm:
- Lập trình ESP32-S3 với ESP-IDF
- Kết nối và giao tiếp với các cảm biến
- Giao thức truyền thông: WiFi, BLE, MQTT, Zigbee
- Xử lý dữ liệu và tích hợp backend

## 🛠️ Công nghệ sử dụng

- **Hardware**: ESP32-S3
- **Framework**: ESP-IDF (Espressif IoT Development Framework)
- **IDE**: Visual Studio Code với ESP-IDF Extension
- **Languages**: C/C++, Arduino (Arduino-ESP32)
- **Backend**: Spring Boot (Java)
- **Database**: PostgreSQL/MySQL
- **Protocols**: MQTT, HTTP, BLE, Zigbee

## 📁 Cấu trúc dự án

```
iot-projects/
├── baitapbuoi2/              # Bài tập buổi 2
├── baitapbuoi6/              # Bài tập buổi 6 - Backend Spring Boot
├── baitapbuoi6ThucHanhIOT/   # Thực hành IoT buổi 6
├── btbuoi2/                  # BLE và các giao thức cơ bản
├── btbuoi3/                  # Tích hợp Backend
├── btbuoi4/                  # Dashboard và hiển thị dữ liệu
├── btbuoi5/                  # Nâng cao
├── example2xx/               # Các ví dụ mẫu (202-213)
├── hello_world/              # Project khởi đầu
├── lab2/, lab3/              # Các bài lab thực hành
└── mqttClient-tcplocalhost1883/  # MQTT Client
```

## 📚 Các Project trong repo

### 🔰 Getting Started

#### [`hello_world`](hello_world/)
- **Mô tả**: Project đơn giản nhất để bắt đầu với Arduino-ESP32 như một ESP-IDF component
- **Công nghệ**: ESP-IDF, Arduino-ESP32
- **Target**: ESP32, ESP32-C2, ESP32-C3, ESP32-C6, ESP32-H2, ESP32-S2, ESP32-S3

### 📡 Communication & Connectivity

#### [`mqttClient-tcplocalhost1883`](mqttClient-tcplocalhost1883/)
- **Mô tả**: MQTT client kết nối với broker local
- **Protocols**: MQTT over TCP
- **Port**: 1883

#### [`btbuoi2/btnhom2ble`](btbuoi2/btnhom2ble/)
- **Mô tả**: Bluetooth Low Energy (BLE) communication
- **Protocols**: BLE

### 🔬 Examples Series

#### [`example202` - `example213`](example202/)
- **Mô tả**: Các project mẫu cơ bản
- **Nội dung**: WiFi, Sensors, Communication protocols
- **Tài liệu**: Mỗi project có README.md riêng

Các example bao gồm:
- **example202-206**: WiFi và network basics
- **example207-208**: Sensor integration
- **example210-211**: MQTT communication
- **example213**: Advanced topics

### 🎓 Lab Assignments

#### Bài tập các buổi học

##### [`baitapbuoi2`](baitapbuoi2/) / [`btbuoi2`](btbuoi2/)
- **Chủ đề**: Giao thức truyền thông cơ bản
- **Nội dung**: BLE, WiFi basics

##### [`baitapbuoi6`](baitapbuoi6/) - Backend Integration
- **Mô tả**: Spring Boot backend cho IoT
- **Tech Stack**: 
  - Spring Boot 3.x
  - Spring Data JPA
  - PostgreSQL/MySQL
- **API Endpoints**: REST API cho quản lý thiết bị IoT
- **Tài liệu**: 
  - [README.md](baitapbuoi6/README.md)
  - [QUICKSTART.md](baitapbuoi6/QUICKSTART.md)
  - [ENVIRONMENT.md](baitapbuoi6/ENVIRONMENT.md)

##### [`btbuoi3`](btbuoi3/) - IoT Backend
- **Mô tả**: Tích hợp backend với ESP32
- **Backend**: Spring Boot
- **Database**: PostgreSQL/MySQL
- **Chi tiết**: [Lab Guide](btbuoi3/iot-backend/iot-backend/noidung/Lab1_SpringBoot_Backend_Detailed%20(1).md)

##### [`btbuoi4`](btbuoi4/) - Dashboard
- **Mô tả**: IoT Dashboard và data visualization
- **Target**: ESP32-S3

##### [`btbuoi5`](btbuoi5/)
- **Mô tả**: Advanced IoT topics

##### [`baitapbuoi6ThucHanhIOT`](baitapbuoi6ThucHanhIOT/)
- **Mô tả**: Thực hành IoT tổng hợp buổi 6

#### Lab Projects

##### [`lab2`](lab2/), [`lab3`](lab3/)
- **Mô tả**: Các bài lab chuyên sâu về sensor và actuator

### 🧩 Components & Libraries

Project sử dụng các component từ ESP Component Registry:
- `espressif/arduino-esp32`: Arduino framework cho ESP32
- `espressif/mdns`: mDNS service discovery
- `espressif/esp_modem`: Modem communication
- `espressif/esp-dsp`: Digital Signal Processing
- `joltwallet/littlefs`: LittleFS filesystem
- Và nhiều component khác...

## 💻 Yêu cầu hệ thống

### Hardware
- ESP32-S3 Development Board
- USB Cable (Type-C)
- Sensors/Actuators (tùy project)

### Software
- [Visual Studio Code](https://code.visualstudio.com/)
- [ESP-IDF Extension for VS Code](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)
- [ESP-IDF v5.0+](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- [Git](https://git-scm.com/)
- [Java JDK 17+](https://www.oracle.com/java/technologies/downloads/) (cho backend projects)
- [PostgreSQL](https://www.postgresql.org/) hoặc [MySQL](https://www.mysql.com/) (cho backend projects)

## 🚀 Hướng dẫn cài đặt

### 1. Clone repository

```bash
git clone <repository-url>
cd iot-projects
```

### 2. Cài đặt ESP-IDF

Làm theo hướng dẫn tại: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

### 3. Cấu hình VS Code

1. Cài đặt ESP-IDF Extension
2. Cấu hình ESP-IDF path trong settings
3. Select COM port cho ESP32-S3

### 4. Build và Flash

```bash
# Di chuyển đến project muốn build
cd hello_world

# Configure project
idf.py menuconfig

# Build project
idf.py build

# Flash to ESP32-S3
idf.py -p COM_PORT flash

# Monitor output
idf.py -p COM_PORT monitor
```

### 5. Chạy Backend (cho các project backend)

```bash
cd baitapbuoi6

# Start backend với PowerShell
.\start-backend.ps1

# Hoặc với Maven
mvn spring-boot:run
```

## 📖 Tài liệu tham khảo

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino-ESP32 Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

## 🤝 Đóng góp

Mọi đóng góp đều được chào đón! Vui lòng:
1. Fork repository
2. Tạo branch mới (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Mở Pull Request

## 📝 License

Các project trong repository này được phân phối dưới các license khác nhau:
- ESP-IDF examples: Apache 2.0
- Arduino-ESP32: LGPL 2.1
- Tham khảo LICENSE file trong từng project để biết chi tiết

## 👥 Tác giả

- **Student**: Hồ Tuấn Phước (2224802010872)
- **Class**: D22CNTT06

## 📧 Liên hệ

Nếu có bất kỳ câu hỏi nào, vui lòng mở issue hoặc liên hệ qua email.

---

**Note**: Repository này được sử dụng cho mục đích học tập và nghiên cứu.