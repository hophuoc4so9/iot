# IoT Control Center - React Web Application

Web application để điều khiển và giám sát thiết bị IoT sử dụng React, Tailwind CSS và MQTT.

## Tính năng

### 🎮 Device Controls (Điều khiển thiết bị)
- **💡 Room Light**: Bật/tắt đèn phòng với switch toggle
- ** RGB LED Color**: 
  - Điều chỉnh màu RGB với 3 slider (R, G, B)
  - Xem trước màu trực tiếp
  - 8 màu preset sẵn (White, Red, Green, Blue, Yellow, Magenta, Cyan, Orange)

### ⚡ System Monitor (Giám sát hệ thống)
- **🔗 Connection Status**: Trạng thái kết nối MQTT
- **🏷️ Namespace**: Hiển thị namespace đang sử dụng (lab/room1)
- **📦 Messages Received**: Đếm số lượng message nhận được
- **🕐 Demo Time**: Hiển thị thời gian thực

## Cài đặt

### Yêu cầu
- Node.js v16+ 
- npm hoặc yarn

### Các bước cài đặt

1. Di chuyển vào thư mục web-iot:
```bash
cd web-iot
```

2. Cài đặt dependencies:
```bash
npm install
```

3. Chạy development server:
```bash
npm run dev
```

4. Mở trình duyệt tại: `http://localhost:5173`

## Cấu hình MQTT

Mặc định ứng dụng kết nối đến MQTT broker tại:
```
ws://10.15.150.248:9002
```

Để thay đổi, sửa biến `WS_URL` trong file `src/App.jsx`:

```javascript
const WS_URL = 'ws://YOUR_MQTT_BROKER_IP:PORT';
```

## MQTT Topics

### Subscribe Topics (Nhận dữ liệu)
- `lab/room1/sensor/state` - Dữ liệu cảm biến
- `lab/room1/device/state` - Trạng thái thiết bị
- `lab/room1/sys/online` - Trạng thái online/offline

### Publish Topics (Gửi lệnh)
- `lab/room1/device/cmd` - Gửi lệnh điều khiển

### Định dạng lệnh điều khiển

**Bật/tắt đèn:**
```json
{ "light": "on" }
{ "light": "off" }
```

**Điều khiển RGB LED:**
```json
{ "led_rgb": [255, 0, 0] }  // Red
{ "led_rgb": [0, 255, 0] }  // Green
{ "led_rgb": [0, 0, 255] }  // Blue
```

## Cấu trúc dự án

```
web-iot/
├── src/
│   ├── components/
│   │   ├── DeviceControls.jsx    # Component điều khiển thiết bị
│   │   └── SystemMonitor.jsx     # Component giám sát hệ thống
│   ├── App.jsx                   # Component chính
│   ├── main.jsx                  # Entry point
│   └── index.css                 # Tailwind CSS
├── index.html
├── package.json
├── tailwind.config.cjs
└── vite.config.js
```

## Build cho Production

```bash
npm run build
```

Các file build sẽ được tạo trong thư mục `dist/`.

Để preview build:
```bash
npm run preview
```

## Công nghệ sử dụng

- **React 18** - UI Framework
- **Vite** - Build tool
- **Tailwind CSS** - CSS Framework
- **MQTT.js** - MQTT client cho browser
- **Chart.js** - Biểu đồ (nếu cần mở rộng)

## Lưu ý

- Đảm bảo MQTT broker đã bật WebSocket listener trên port 9002
- Kiểm tra tường lửa không chặn kết nối WebSocket
- Console log sẽ hiển thị chi tiết về kết nối và messages

## Troubleshooting

**Không kết nối được MQTT:**
- Kiểm tra IP và port của MQTT broker
- Kiểm tra MQTT broker đã enable WebSocket
- Kiểm tra firewall và network

**UI không cập nhật khi nhận message:**
- Mở Developer Console (F12) xem log
- Kiểm tra định dạng message từ device

## License

MIT
