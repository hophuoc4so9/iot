# 🧪 Lab 5 (Chi tiết): API Điều khiển Thiết bị IoT

> **Mục tiêu cuối buổi:**  
> - Hiểu cơ chế backend **publish lệnh** xuống thiết bị qua MQTT.  
> - Xây dựng API `POST /api/devices/{id}/command` để gửi lệnh bật/tắt.  
> - Thiết bị giả lập nhận lệnh và in ra console.  

---

## 🧭 0) Ngữ cảnh & phạm vi Lab
Lab 4: dữ liệu từ thiết bị đã gửi lên DB.  
Lab 5: ta đi theo chiều ngược lại → **backend gửi lệnh xuống thiết bị**.

Luồng hoạt động:
```
Client (Postman) ──► Backend API ──► Publish MQTT ──► Broker ──► Device (subscriber)
```

---

## ✅ 1) Chuẩn bị
- Backend đã kết nối broker (Lab 2)  
- DB có device (Lab 1,3)  
- Python script giả lập thiết bị (subscriber)  

---

## 🧱 2) Định nghĩa topic cho điều khiển
Quy ước:  
```
iot/device/{id}/command
```
Ví dụ: `iot/device/1/command` để gửi lệnh cho device id=1.

Payload: JSON, ví dụ:
```json
{"action": "ON"}
```
hoặc
```json
{"action": "OFF"}
```

---

## 📚 3) Service để publish MQTT

`src/main/java/com/example/iotbackend/service/CommandService.java`
```java
package com.example.iotbackend.service;

import com.example.iotbackend.config.MqttConfig;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

@Service
public class CommandService {
  private static final Logger log = LoggerFactory.getLogger(CommandService.class);
  private final MqttClient mqttClient;
  private final MqttConfig cfg;

  public CommandService(MqttClient mqttClient, MqttConfig cfg) {
    this.mqttClient = mqttClient;
    this.cfg = cfg;
  }

  public void sendCommand(Long deviceId, String action) throws MqttException {
    String topic = "iot/device/" + deviceId + "/command";
    String payload = String.format("{\"action\":\"%s\"}", action);

    MqttMessage msg = new MqttMessage(payload.getBytes());
    msg.setQos(cfg.getQos());
    mqttClient.publish(topic, msg);
    log.info("Published command={} to topic={}", action, topic);
  }
}
```

> Lưu ý: Ở Lab 2 khi tạo `MqttClient` (Paho) trong `PahoMqttService`, ta nên đánh dấu `@Bean` để inject dùng lại.

Ví dụ trong `PahoMqttService`:
```java
@Bean
public MqttClient mqttClient() throws MqttException {
  String brokerUrl = "tcp://" + cfg.getHost() + ":" + cfg.getPort();
  MqttClient client = new MqttClient(brokerUrl, cfg.getClientId(), new MemoryPersistence());
  client.connect();
  return client;
}
```

---

## 🌐 4) API Controller cho command

`src/main/java/com/example/iotbackend/controller/CommandController.java`
```java
package com.example.iotbackend.controller;

import com.example.iotbackend.service.CommandService;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/devices")
public class CommandController {
  private final CommandService commandService;

  public CommandController(CommandService commandService) {
    this.commandService = commandService;
  }

  @PostMapping("/{id}/command")
  public ResponseEntity<String> sendCommand(@PathVariable Long id, @RequestBody Map<String, String> body) {
    try {
      String action = body.get("action");
      commandService.sendCommand(id, action);
      return ResponseEntity.ok("Command sent: " + action);
    } catch (MqttException e) {
      return ResponseEntity.status(500).body("Failed to send command: " + e.getMessage());
    }
  }
}
```

---

## 🛰️ 5) Thiết bị giả lập (subscriber)

### Python Script: `device_subscriber.py`
```python
import time
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
DEVICE_ID = 1
TOPIC = f"iot/device/{DEVICE_ID}/command"

def on_message(client, userdata, msg):
    print(f"[Device {DEVICE_ID}] Received command: {msg.payload.decode()}")

client = mqtt.Client()
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC, qos=1)

print(f"[Device {DEVICE_ID}] Listening for commands on topic {TOPIC}...")
client.loop_forever()
```

Chạy subscriber:
```bash
python device_subscriber.py
```

---

## ▶️ 6) Gửi lệnh từ backend

### 6.1 Dùng Postman
- Method: `POST`  
- URL: `http://localhost:8080/api/devices/1/command`  
- Body (JSON):
```json
{
  "action": "ON"
}
```

### 6.2 Dùng cURL
```bash
curl -X POST http://localhost:8080/api/devices/1/command \
  -H "Content-Type: application/json" \
  -d '{"action":"OFF"}'
```

Kỳ vọng:
- Backend log: `Published command=OFF to topic=iot/device/1/command`
- Subscriber log: `[Device 1] Received command: {"action":"OFF"}`

---

## 🛠️ 7) Lỗi thường gặp
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| `MqttException: Client is not connected` | Chưa connect MQTT hoặc disconnect | Đảm bảo client connect trước khi publish |
| Subscriber không nhận | Sai topic hoặc deviceId | Kiểm tra `topic` khớp `iot/device/{id}/command` |
| API trả 500 | Broker chưa chạy | Khởi động broker trước |
| Subscriber chạy nhưng không in | Sai QoS hoặc chưa subscribe đúng | Dùng `qos=1`, kiểm tra subscribe đúng topic |

---

## 🏁 Kết quả cuối cùng
- API `POST /api/devices/{id}/command` hoạt động.  
- Backend publish lệnh MQTT thành công.  
- Thiết bị giả lập nhận lệnh và in ra console.  

---

## 📚 Bài tập củng cố
1) Thêm tham số `duration` (giây) vào payload, ví dụ `{"action":"ON","duration":10}`.  
2) Subscriber in ra “Device bật trong 10s” rồi tự động “tắt”.  
3) Thêm API `POST /api/devices/{id}/command/custom` cho phép gửi payload JSON tùy ý.  
