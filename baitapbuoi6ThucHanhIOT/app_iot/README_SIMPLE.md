# IoT Control Center - Flutter App

Ứng dụng Flutter để điều khiển thiết bị IoT qua MQTT - Chỉ có chức năng điều khiển đèn.

## 📱 Tính năng

### 🎮 Device Controls (Điều khiển thiết bị)
- **💡 Room Light**: Bật/tắt đèn phòng với Switch toggle

### ⚡ System Monitor (Giám sát hệ thống)
- **🔗 Connection Status**: Trạng thái kết nối MQTT
- **🏷️ Namespace**: Hiển thị namespace đang sử dụng (lab/room1)
- **📦 Messages Received**: Đếm số lượng message nhận được
- **🕐 Demo Time**: Hiển thị thời gian thực

## 🚀 Cài đặt

### Yêu cầu
- Flutter SDK 3.9.2 trở lên
- Dart SDK
- Android Studio / VS Code
- Device hoặc Emulator

### Các bước cài đặt

1. Di chuyển vào thư mục app_iot:
```bash
cd app_iot
```

2. Cài đặt dependencies:
```bash
flutter pub get
```

3. Chạy ứng dụng:
```bash
flutter run
```

## ⚙️ Cấu hình MQTT

Mặc định ứng dụng kết nối đến MQTT broker tại:
- **Broker**: `10.15.150.248`
- **Port**: `1883`
- **Namespace**: `lab/room1`

Để thay đổi, sửa các constant trong file `lib/screens/dashboard_screen_simple.dart`:

```dart
static const String broker = '10.15.150.248';
static const int port = 1883;
static const String namespace = 'lab/room1';
```

## 📡 MQTT Topics

### Subscribe Topics (Nhận dữ liệu)
- `lab/room1/device/state` - Trạng thái thiết bị (đèn)
- `lab/room1/sys/online` - Trạng thái online/offline

### Publish Topics (Gửi lệnh)
- `lab/room1/device/cmd` - Gửi lệnh điều khiển

### Định dạng lệnh điều khiển

**Bật/tắt đèn:**
```json
{ "light": "on" }
{ "light": "off" }
```

## 📁 Cấu trúc dự án

```
app_iot/
├── lib/
│   ├── screens/
│   │   └── dashboard_screen_simple.dart  # Màn hình chính
│   └── main.dart                         # Entry point
├── pubspec.yaml                          # Dependencies
└── README_SIMPLE.md                      # Tài liệu này
```

## 📦 Dependencies

```yaml
dependencies:
  flutter:
    sdk: flutter
  mqtt_client: ^10.2.0     # MQTT client
  intl: ^0.19.0            # Date formatting
  cupertino_icons: ^1.0.8  # iOS icons
```

## 🎨 UI Features

- **Gradient Background**: Màu gradient tím đẹp mắt
- **Material Cards**: Card design hiện đại
- **Responsive Layout**: Tự động điều chỉnh theo màn hình
- **Real-time Updates**: Cập nhật trạng thái theo thời gian thực
- **Status Indicators**: Hiển thị trạng thái kết nối bằng màu sắc

## 🔧 Build cho Production

### Android APK:
```bash
flutter build apk --release
```

### Android App Bundle:
```bash
flutter build appbundle --release
```

### iOS:
```bash
flutter build ios --release
```

## 💡 So sánh với Web App

App Flutter này có giao diện và chức năng tương tự web-iot nhưng:
- ✅ Chỉ có điều khiển đèn (không có Fan và RGB LED)
- ✅ Sử dụng MQTT trực tiếp (không qua WebSocket)
- ✅ UI native với Material Design
- ✅ Có thể chạy trên mobile (Android/iOS)

## 🐛 Troubleshooting

**Không kết nối được MQTT:**
- Kiểm tra IP và port của MQTT broker
- Kiểm tra device/emulator có kết nối mạng
- Kiểm tra firewall không chặn port 1883
- Thử ping broker từ terminal: `ping 10.15.150.248`

**App bị crash khi khởi động:**
- Chạy `flutter clean`
- Chạy `flutter pub get`
- Rebuild lại app

**UI không cập nhật:**
- Kiểm tra console log để xem MQTT messages
- Kiểm tra định dạng JSON từ device

## 📝 Notes

- App tự động kết nối MQTT khi khởi động
- Switch chỉ hoạt động khi đã kết nối MQTT
- Message count tự động tăng khi nhận message mới
- Thời gian hiển thị là thời gian local của device

## 📄 License

MIT
