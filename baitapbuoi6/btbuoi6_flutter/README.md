# btbuoi6_flutter

IoT Device Dashboard - Flutter Mobile App

## 🚀 Getting Started

### Cài đặt

```bash
flutter pub get
```

### Chạy ứng dụng

```bash
# Chạy trên Android Emulator
flutter run

# Chạy trên thiết bị cụ thể
flutter run -d <device_id>

# Build APK
flutter build apk --release
```

### Cấu hình Backend URL

Mở `lib/main.dart` và thay đổi `_baseUrl`:

```dart
// Cho Android Emulator
final _baseUrl = 'http://10.0.2.2:8080';

// Cho iOS Simulator
final _baseUrl = 'http://localhost:8080';

// Cho thiết bị thật (thay IP thật của máy bạn)
final _baseUrl = 'http://192.168.1.100:8080';
```

## 📱 Tính năng

- ✅ Xem danh sách thiết bị IoT
- ✅ Thêm thiết bị mới
- ✅ Gửi lệnh điều khiển thiết bị
- ✅ Xem dữ liệu telemetry
- ✅ Pull to refresh
- ✅ Material Design 3

## 🎨 UI Components

- Material 3 Design
- Cards với elevation
- Bottom Sheets
- Dialogs
- SnackBars
- RefreshIndicator

## 📦 Dependencies

- `http: ^1.2.0` - HTTP requests
- `flutter/material` - Material Design widgets

---

Đảm bảo Backend đang chạy tại port 8080 trước khi test app!
