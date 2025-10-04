# 🧪 Lab 2 (Chi tiết): Tích hợp MQTT Client vào Spring Boot

> **Mục tiêu cuối buổi:** Backend **kết nối được đến MQTT broker**, **subscribe** topic `iot/demo/temp` và **in dữ liệu ra console** khi nhận message từ thiết bị/giả lập.

---

## 🧭 0) Ngữ cảnh & phạm vi Lab
Trong Lab 1, ta đã có **backend + DB**. Ở Lab 2, ta thêm **MQTT Client** để nhận dữ liệu **telemetry** qua **broker**. (Việc lưu vào DB sẽ thực hiện ở Lab sau.)

Luồng cơ bản:
```
Publisher (MQTT Explorer / script Python) ──► MQTT Broker ──► Backend (subscribe) ──► Console log
```

---

## ✅ 1) Yêu cầu môi trường
- Dự án Spring Boot (từ Lab 1)
- **Broker MQTT** (ví dụ: Mosquitto, EMQX, HiveMQ)
  - Cổng mặc định TCP: `1883`
  - Nếu dùng Docker cho Mosquitto:
    ```bash
    docker run -it --name mosquitto -p 1883:1883 eclipse-mosquitto
    ```
- Postman/cURL (không bắt buộc cho Lab này)
- Tuỳ chọn: **Python 3** để chạy script publisher

> Có thể dùng **MQTT Explorer** (GUI) để publish message thử.

---

## 🧩 2) Chọn thư viện MQTT Client
Ta cung cấp **2 lựa chọn** để tích hợp vào Spring Boot:

- **Option A (khuyến nghị): HiveMQ MQTT Client** – hiện đại, async, hỗ trợ MQTT v3/v5.
- **Option B: Eclipse Paho** – phổ biến, đơn giản, dễ dùng.

> Tuỳ bài giảng, chọn **một** trong hai option. Dưới đây là hướng dẫn **cả hai**.

---

## 🛠️ 3) Cấu hình chung (application.yml)
Tạo/sửa `src/main/resources/application.yml` và **thêm** block cấu hình MQTT (dùng được cho cả 2 option):
```yaml
mqtt:
  host: localhost
  port: 1883
  username: ""   # nếu broker không yêu cầu auth -> để rỗng
  password: ""   # nếu có auth -> điền vào
  clientId: iot-backend-${random.uuid}
  topicTemp: iot/demo/temp
  qos: 1
  keepAlive: 30
```

---

## 🚀 OPTION A — Dùng HiveMQ MQTT Client

### 3A.1 Thêm dependency vào `pom.xml`
```xml
<dependency>
  <groupId>com.hivemq</groupId>
  <artifactId>hivemq-mqtt-client</artifactId>
  <version>1.3.0</version>
</dependency>
```

### 3A.2 Tạo cấu hình & service MQTT

**`MqttConfig.java`**
`src/main/java/com/example/iotbackend/config/MqttConfig.java`
```java
package com.example.iotbackend.config;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MqttConfig {

  @Value("${mqtt.host}") private String host;
  @Value("${mqtt.port}") private int port;
  @Value("${mqtt.username:}") private String username;
  @Value("${mqtt.password:}") private String password;
  @Value("${mqtt.clientId}") private String clientId;
  @Value("${mqtt.topicTemp}") private String topicTemp;
  @Value("${mqtt.qos:1}") private int qos;
  @Value("${mqtt.keepAlive:30}") private int keepAlive;

  public String getHost() { return host; }
  public int getPort() { return port; }
  public String getUsername() { return username; }
  public String getPassword() { return password; }
  public String getClientId() { return clientId; }
  public String getTopicTemp() { return topicTemp; }
  public int getQos() { return qos; }
  public int getKeepAlive() { return keepAlive; }
}
```

**`HiveMqttService.java`**
`src/main/java/com/example/iotbackend/mqtt/HiveMqttService.java`
```java
package com.example.iotbackend.mqtt;

import com.example.iotbackend.config.MqttConfig;
import com.hivemq.client.mqtt.MqttClient;
import com.hivemq.client.mqtt.MqttClientBuilder;
import com.hivemq.client.mqtt.datatypes.MqttQos;
import com.hivemq.client.mqtt.mqtt3.Mqtt3AsyncClient;
import jakarta.annotation.PreDestroy;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

import java.nio.charset.StandardCharsets;
import java.util.Optional;

@Service
public class HiveMqttService {
  private static final Logger log = LoggerFactory.getLogger(HiveMqttService.class);
  private final MqttConfig cfg;
  private Mqtt3AsyncClient client;

  public HiveMqttService(MqttConfig cfg) {
    this.cfg = cfg;
  }

  @Bean
  public ApplicationRunner mqttRunner() {
    return args -> {
      connectAndSubscribe();
    };
  }

  private void connectAndSubscribe() {
    MqttClientBuilder builder = MqttClient.builder()
        .useMqttVersion3()
        .identifier(cfg.getClientId())
        .serverHost(cfg.getHost())
        .serverPort(cfg.getPort());

    client = builder.buildAsync();

    client.connectWith()
        .keepAlive(cfg.getKeepAlive())
        .simpleAuth(auth -> {
          if (!Optional.ofNullable(cfg.getUsername()).orElse("").isBlank()) {
            auth.username(cfg.getUsername())
                .password(StandardCharsets.UTF_8.encode(Optional.ofNullable(cfg.getPassword()).orElse("")));
          }
        })
        .send()
        .whenComplete((ack, ex) -> {
          if (ex != null) {
            log.error("MQTT connect failed: {}", ex.getMessage(), ex);
            return;
          }
          log.info("MQTT connected as clientId={}", cfg.getClientId());

          client.subscribeWith()
              .topicFilter(cfg.getTopicTemp())
              .qos(MqttQos.fromCode(cfg.getQos()))
              .callback(publish -> {
                String payload = new String(publish.getPayloadAsBytes(), StandardCharsets.UTF_8);
                log.info("[HiveMQ] Received topic='{}' payload={}", publish.getTopic(), payload);
              })
              .send()
              .whenComplete((subAck, subEx) -> {
                if (subEx != null) {
                  log.error("MQTT subscribe failed: {}", subEx.getMessage(), subEx);
                } else {
                  log.info("Subscribed to topic {}", cfg.getTopicTemp());
                }
              });
        });
  }

  @PreDestroy
  public void shutdown() {
    if (client != null) {
      client.disconnect();
      log.info("MQTT disconnected");
    }
  }
}
```

> ✅ Khi ứng dụng chạy, nó sẽ **kết nối** đến broker và **subscribe** `iot/demo/temp`. Khi có message, log sẽ in ra console.

---

## 🚀 OPTION B — Dùng Eclipse Paho

### 3B.1 Thêm dependency vào `pom.xml`
```xml
<dependency>
  <groupId>org.eclipse.paho</groupId>
  <artifactId>org.eclipse.paho.client.mqttv3</artifactId>
  <version>1.2.5</version>
</dependency>
```

### 3B.2 Tạo service MQTT với Paho

**`PahoMqttService.java`**
`src/main/java/com/example/iotbackend/mqtt/PahoMqttService.java`
```java
package com.example.iotbackend.mqtt;

import com.example.iotbackend.config.MqttConfig;
import jakarta.annotation.PreDestroy;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

@Service
public class PahoMqttService implements MqttCallback {
  private static final Logger log = LoggerFactory.getLogger(PahoMqttService.class);
  private final MqttConfig cfg;
  private MqttClient client;

  public PahoMqttService(MqttConfig cfg) {
    this.cfg = cfg;
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

    if (cfg.getUsername() != null && !cfg.getUsername().isBlank()) {
      options.setUserName(cfg.getUsername());
      options.setPassword(cfg.getPassword().toCharArray());
    }

    client.setCallback(this);
    client.connect(options);
    log.info("Connected to MQTT broker {}", brokerUrl);

    client.subscribe(cfg.getTopicTemp(), cfg.getQos());
    log.info("Subscribed to topic {}", cfg.getTopicTemp());
  }

  @PreDestroy
  public void shutdown() {
    try {
      if (client != null && client.isConnected()) {
        client.disconnect();
        log.info("MQTT disconnected");
      }
    } catch (MqttException e) {
      log.error("Error disconnecting MQTT", e);
    }
  }

  @Override
  public void connectionLost(Throwable cause) {
    log.warn("MQTT connection lost: {}", cause.getMessage());
  }

  @Override
  public void messageArrived(String topic, MqttMessage message) {
    String payload = new String(message.getPayload());
    log.info("[Paho] Received topic='{}' payload={}", topic, payload);
  }

  @Override
  public void deliveryComplete(IMqttDeliveryToken token) {
    // not used for subscriber in this lab
  }
}
```

> ✅ Khi chạy app, service sẽ **kết nối** và **subscribe** topic `iot/demo/temp`, mọi message sẽ được in ra console.

---

## 🛰️ 4) Tạo Publisher (Giả lập thiết bị)

### 4.1 Dùng **Python** (thư viện `paho-mqtt`)
Cài đặt:
```bash
pip install paho-mqtt
```
Script `publisher.py`:
```python
import time, json, random
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC = "iot/demo/temp"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

try:
    while True:
        payload = {"temp": round(20 + random.random()*10, 2)}
        client.publish(TOPIC, json.dumps(payload), qos=1)
        print("Published:", payload)
        time.sleep(2)
except KeyboardInterrupt:
    print("Stopped")
finally:
    client.disconnect()
```

Chạy:
```bash
python publisher.py
```

### 4.2 Dùng **mosquitto_pub** (CLI)
```bash
mosquitto_pub -h localhost -p 1883 -t iot/demo/temp -m '{"temp": 25.3}' -q 1
```

### 4.3 Dùng **MQTT Explorer** (GUI)
- Kết nối tới `localhost:1883`
- Chọn **Publish** → Topic: `iot/demo/temp`
- Message: `{"temp": 24.7}` → **Publish**

---

## ▶️ 5) Chạy & kiểm thử
1. Đảm bảo broker đang chạy (`1883` mở, có thể telnet: `telnet localhost 1883`).
2. Chạy Spring Boot:
   ```bash
   ./mvnw spring-boot:run
   ```
3. Mở terminal khác, chạy **publisher** (Python hoặc mosquitto_pub hoặc MQTT Explorer).
4. Quan sát **console log** của ứng dụng Spring Boot:
   - Thấy log: `MQTT connected ...`
   - Thấy log: `Subscribed to topic iot/demo/temp`
   - Khi publish: log in ra nội dung payload nhận được.

---

## 🔒 6) (Tuỳ chọn) Bật TLS/SSL & Auth
- Nếu broker yêu cầu TLS (cổng 8883), cần cấu hình **SSL context** tương ứng.
- Với HiveMQ Client: sử dụng `sslWithDefaultConfig()` trong builder.
- Với Paho: dùng `ssl://host:8883` và cấu hình `SSLSocketFactory` trong `MqttConnectOptions`.
- Thêm `username/password` nếu broker bật Basic Auth.

*(Phần TLS chi tiết có thể để Lab bảo mật riêng.)*

---

## 🧰 7) Lỗi thường gặp & cách xử lý
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| `Connection refused` | Broker chưa chạy / sai host/port | Kiểm tra container broker, firewall, port |
| Connect OK nhưng không nhận message | Subscribe sai topic hoặc QoS | Kiểm tra đúng `iot/demo/temp`, đúng QoS (0/1/2) |
| Broker yêu cầu auth | Thiếu username/password | Điền vào `application.yml` |
| Client ID trùng | Nhiều client dùng cùng `clientId` | Đặt `clientId` khác nhau (dùng `${random.uuid}`) |
| Dùng TLS nhưng fail | Sai cổng/CA/cert | Kiểm tra cổng 8883 và chứng chỉ |
| Không thấy log | Logger ở mức khác | Đảm bảo `INFO` bật trong `logback`/`application.yml` |

---

## 🏁 Kết quả cuối cùng
- Ứng dụng **kết nối thành công** tới MQTT broker.
- **Subscribe** topic `iot/demo/temp` **thành công**.
- **In ra console** payload nhận được từ publisher.

---

## 📚 Bài tập củng cố (5–10 phút)
1) Tạo thêm topic `iot/demo/hum` và subscribe đồng thời cả `temp` & `hum`.  
2) Parse JSON payload (temp) và **log cảnh báo** nếu `temp > 30°C`.  
3) Viết **API POST** để **publish** lệnh điều khiển (ví dụ `/api/mqtt/publish`) và test publish ra topic `iot/demo/cmd`.

> Gợi ý: Với HiveMQ Client dùng `client.publishWith()...send()`; với Paho dùng `client.publish(topic, payload, qos, retained)`.
