# 🧪 Lab 4 (Chi tiết): Gửi dữ liệu Telemetry và Lưu vào Database

> **Mục tiêu cuối buổi:** Xây dựng pipeline dữ liệu IoT cơ bản:
> - Thiết bị (giả lập) **publish** dữ liệu nhiệt độ/độ ẩm.  
> - Backend **subscribe** topic MQTT, nhận dữ liệu.  
> - Backend **lưu dữ liệu vào bảng `telemetry` trong DB**.  
> - Có thể **kiểm tra DB** để thấy dữ liệu được ghi lại.  

---

## 🧭 0) Ngữ cảnh & phạm vi Lab
Lab 2: backend đã subscribe được topic.  
Lab 3: backend đã có API quản lý thiết bị.  
Trong Lab 4: ta kết nối cả hai → dữ liệu từ thiết bị sẽ được ghi xuống DB.

Luồng hoạt động:
```
Publisher (ESP32/Python) ──► MQTT Broker ──► Backend (subscribe) ──► DB (telemetry)
```

---

## ✅ 1) Chuẩn bị
- Code Lab 1 + Lab 2 (backend kết nối DB + MQTT)  
- Bảng `telemetry` đã có trong DB (Lab 1 đã tạo entity `Telemetry`)  
- MQTT Broker chạy trên `localhost:1883`  
- Publisher giả lập (Python script, MQTT Explorer hoặc mosquitto_pub)

---

## 🧱 2) Cập nhật Entity Telemetry (nếu cần)

`src/main/java/com/example/iotbackend/model/Telemetry.java`
```java
package com.example.iotbackend.model;

import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "telemetry")
public class Telemetry {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;

  @ManyToOne(optional = false)
  @JoinColumn(name = "device_id")
  private Device device;

  private Instant ts = Instant.now();

  @Column(columnDefinition = "text")
  private String data; // JSON string: {"temp": 26.5, "hum": 70}

  // getters/setters ...
}
```

---

## 📚 3) Repository cho Telemetry

`src/main/java/com/example/iotbackend/repository/TelemetryRepository.java`
```java
package com.example.iotbackend.repository;

import com.example.iotbackend.model.Telemetry;
import org.springframework.data.jpa.repository.JpaRepository;

public interface TelemetryRepository extends JpaRepository<Telemetry, Long> {
}
```

---

## 🧠 4) Service xử lý Telemetry

`src/main/java/com/example/iotbackend/service/TelemetryService.java`
```java
package com.example.iotbackend.service;

import com.example.iotbackend.model.Device;
import com.example.iotbackend.model.Telemetry;
import com.example.iotbackend.repository.DeviceRepository;
import com.example.iotbackend.repository.TelemetryRepository;
import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.Optional;

@Service
public class TelemetryService {
  private final TelemetryRepository telemetryRepo;
  private final DeviceRepository deviceRepo;

  public TelemetryService(TelemetryRepository telemetryRepo, DeviceRepository deviceRepo) {
    this.telemetryRepo = telemetryRepo;
    this.deviceRepo = deviceRepo;
  }

  public void saveTelemetry(Long deviceId, String jsonData) {
    Optional<Device> deviceOpt = deviceRepo.findById(deviceId);
    if (deviceOpt.isPresent()) {
      Telemetry t = new Telemetry();
      t.setDevice(deviceOpt.get());
      t.setTs(Instant.now());
      t.setData(jsonData);
      telemetryRepo.save(t);
    } else {
      System.out.println("Device not found with id=" + deviceId);
    }
  }
}
```

---

## 🌐 5) MQTT Subscriber: Nhận & Lưu dữ liệu

### Giả định topic: `iot/device/{id}/telemetry`
Ví dụ: `iot/device/1/telemetry` (id thiết bị = 1).  

**Cập nhật service MQTT (ví dụ với Paho):**

`src/main/java/com/example/iotbackend/mqtt/PahoMqttService.java`
```java
// ... import

@Service
public class PahoMqttService implements MqttCallback {
  private static final Logger log = LoggerFactory.getLogger(PahoMqttService.class);
  private final MqttConfig cfg;
  private final TelemetryService telemetryService;
  private MqttClient client;

  public PahoMqttService(MqttConfig cfg, TelemetryService telemetryService) {
    this.cfg = cfg;
    this.telemetryService = telemetryService;
  }

  @Bean
  public ApplicationRunner mqttRunner() {
    return args -> connectAndSubscribe();
  }

  private void connectAndSubscribe() throws MqttException {
    String brokerUrl = "tcp://" + cfg.getHost() + ":" + cfg.getPort();
    client = new MqttClient(brokerUrl, cfg.getClientId(), new MemoryPersistence());

    MqttConnectOptions options = new MqttConnectOptions();
    options.setCleanSession(true);
    options.setAutomaticReconnect(true);
    options.setKeepAliveInterval(cfg.getKeepAlive());

    client.setCallback(this);
    client.connect(options);
    log.info("Connected to MQTT broker {}", brokerUrl);

    // Sub wildcard để nhận mọi thiết bị
    client.subscribe("iot/device/+/telemetry", cfg.getQos());
    log.info("Subscribed to iot/device/+/telemetry");
  }

  @Override
  public void messageArrived(String topic, MqttMessage message) {
    String payload = new String(message.getPayload());
    log.info("Received topic={} payload={}", topic, payload);

    // Extract deviceId từ topic: iot/device/{id}/telemetry
    try {
      String[] parts = topic.split("/");
      Long deviceId = Long.parseLong(parts[2]);
      telemetryService.saveTelemetry(deviceId, payload);
    } catch (Exception e) {
      log.error("Failed to save telemetry", e);
    }
  }

  @Override
  public void connectionLost(Throwable cause) {
    log.warn("MQTT connection lost: {}", cause.getMessage());
  }

  @Override
  public void deliveryComplete(IMqttDeliveryToken token) {}
}
```

> ✅ Giờ khi có message MQTT, backend sẽ tự động lưu vào DB.

---

## 🛰️ 6) Publisher giả lập (Python)

`publisher.py`
```python
import time, json, random
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
DEVICE_ID = 1
TOPIC = f"iot/device/{DEVICE_ID}/telemetry"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

try:
    while True:
        payload = {
            "temp": round(20 + random.random()*10, 2),
            "hum": round(40 + random.random()*20, 2)
        }
        client.publish(TOPIC, json.dumps(payload), qos=1)
        print("Published:", payload)
        time.sleep(3)
except KeyboardInterrupt:
    print("Stopped")
finally:
    client.disconnect()
```

Chạy:
```bash
python publisher.py
```

---

## ▶️ 7) Kiểm tra kết quả

### 7.1 Quan sát console backend
Sẽ thấy log:
```
Received topic=iot/device/1/telemetry payload={"temp":27.5,"hum":55.2}
```

### 7.2 Kiểm tra DB
PostgreSQL:
```sql
SELECT * FROM telemetry ORDER BY ts DESC LIMIT 5;
```

Ví dụ kết quả:
```
 id | device_id |          ts           |             data
----+-----------+-----------------------+------------------------------
  1 |         1 | 2025-09-15 12:30:01   | {"temp":27.5,"hum":55.2}
  2 |         1 | 2025-09-15 12:30:04   | {"temp":28.1,"hum":54.7}
```

---

## 🛠️ 8) Lỗi thường gặp
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| `Device not found` | DB chưa có device id tương ứng | Tạo trước device trong bảng `devices` |
| Không nhận message | Sai topic subscribe | Kiểm tra đúng `iot/device/+/telemetry` |
| JSON không lưu | payload không đúng format | Đảm bảo publisher gửi JSON hợp lệ |
| Hibernate lỗi | Bảng telemetry chưa tạo | Xoá DB, restart để Hibernate tạo lại |

---

## 🏁 Kết quả cuối cùng
- Thiết bị publish dữ liệu lên topic thành công.  
- Backend subscribe nhận message.  
- Backend lưu dữ liệu telemetry vào DB.  
- Có thể kiểm tra dữ liệu bằng SQL.  

---

## 📚 Bài tập củng cố
1) Tạo thêm thiết bị (device_id=2), publish telemetry song song. Quan sát DB lưu đúng theo từng id.  
2) Thêm API `GET /api/devices/{id}/telemetry` trả về dữ liệu gần nhất của thiết bị.  
3) Thêm logic cảnh báo: nếu `temp > 30°C`, log cảnh báo trong backend.  
