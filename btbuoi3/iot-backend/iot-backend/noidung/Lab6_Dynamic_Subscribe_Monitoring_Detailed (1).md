# 🧪 Lab 6 (Chi tiết): API Đăng ký Topic Động & Giám sát Dữ liệu (REST / WebSocket)

> **Mục tiêu cuối buổi:**  
> - Backend có **API để đăng ký subscribe vào topic MQTT một cách động** (không cần hard-code).  
> - **Giám sát dữ liệu nhận được**: hiển thị qua **REST (polling)** hoặc **WebSocket (realtime push)**.

---

## 🧭 0) Ngữ cảnh & Phạm vi
- Từ Lab 2–5, backend đã kết nối broker, nhận telemetry, lưu DB, và publish lệnh.  
- Ở Lab 6, ta tăng **tính linh hoạt**: cho phép **đăng ký/huỷ đăng ký** topic **khi đang chạy** và **quan sát dữ liệu** theo thời gian thực.

Luồng hoạt động:
```
Client ──► REST API đăng ký topic ──► Backend subscribe topic trên broker
                                           │
                                           ├─► Expose REST API xem log mới nhất (polling)
                                           └─► Đẩy dữ liệu qua WebSocket/SSE (realtime)
```

---

## ✅ 1) Chuẩn bị
- Code từ Lab 2 (Paho/HiveMQ) – bên dưới minh hoạ bằng **Paho**.  
- Spring Boot 3.x, đã có `spring-boot-starter-web`.  
- MQTT Broker (Mosquitto/EMQX) chạy `localhost:1883`.

---

## 🧱 2) Quản lý Subscription động & Bộ nhớ tạm cho message
Ta cần:
1. **SubscriptionManager**: đăng ký/huỷ đăng ký topic động với MQTT client.  
2. **MessageStore**: lưu **tạm thời** các message theo từng topic để phục vụ REST/WebSocket (ví dụ giữ 200 message gần nhất).

### 2.1 `MessageStore` (in-memory, vòng đệm theo topic)
`src/main/java/com/example/iotbackend/mqtt/MessageStore.java`
```java
package com.example.iotbackend.mqtt;

import org.springframework.stereotype.Component;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedDeque;

@Component
public class MessageStore {
  private static final int MAX_PER_TOPIC = 200;

  // topic -> deque of payloads
  private final Map<String, Deque<String>> store = new ConcurrentHashMap<>();

  public void append(String topic, String payload) {
    store.computeIfAbsent(topic, t -> new ConcurrentLinkedDeque<>()).addLast(payload);
    Deque<String> q = store.get(topic);
    while (q.size() > MAX_PER_TOPIC) {
      q.pollFirst();
    }
  }

  public List<String> recent(String topic, int limit) {
    Deque<String> q = store.getOrDefault(topic, new ConcurrentLinkedDeque<>());
    List<String> all = new ArrayList<>(q);
    int size = all.size();
    int from = Math.max(0, size - limit);
    return all.subList(from, size);
  }

  public Set<String> topics() {
    return store.keySet();
  }

  public void clear(String topic) {
    store.remove(topic);
  }
}
```

### 2.2 `SubscriptionManager` (đăng ký/huỷ đăng ký topic)
`src/main/java/com/example/iotbackend/mqtt/SubscriptionManager.java`
```java
package com.example.iotbackend.mqtt;

import com.example.iotbackend.config.MqttConfig;
import org.eclipse.paho.client.mqttv3.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

@Component
public class SubscriptionManager {
  private static final Logger log = LoggerFactory.getLogger(SubscriptionManager.class);
  private final MqttClient client;
  private final MqttConfig cfg;
  private final MessageStore messageStore;

  private final Set<String> currentTopics = ConcurrentHashMap.newKeySet();

  public SubscriptionManager(MqttClient client, MqttConfig cfg, MessageStore messageStore) throws MqttException {
    this.client = client;
    this.cfg = cfg;
    this.messageStore = messageStore;

    this.client.setCallback(new MqttCallback() {
      @Override public void connectionLost(Throwable cause) {
        log.warn("MQTT lost: {}", cause.getMessage());
      }
      @Override public void messageArrived(String topic, MqttMessage message) {
        String payload = new String(message.getPayload());
        log.info("[SUB] {} -> {}", topic, payload);
        messageStore.append(topic, payload);
      }
      @Override public void deliveryComplete(IMqttDeliveryToken token) {}
    });

    if (!client.isConnected()) {
      String brokerUrl = "tcp://" + cfg.getHost() + ":" + cfg.getPort();
      client.connect();
      log.info("Connected MQTT at {}", brokerUrl);
    }
  }

  public synchronized void subscribe(String topic) throws MqttException {
    if (!currentTopics.contains(topic)) {
      client.subscribe(topic, cfg.getQos());
      currentTopics.add(topic);
      log.info("Subscribed: {}", topic);
    }
  }

  public synchronized void unsubscribe(String topic) throws MqttException {
    if (currentTopics.contains(topic)) {
      client.unsubscribe(topic);
      currentTopics.remove(topic);
      log.info("Unsubscribed: {}", topic);
    }
  }

  public Set<String> list() {
    return Set.copyOf(currentTopics);
  }
}
```

> **Lưu ý:** Cần có `@Bean` tạo `MqttClient` (đã hướng dẫn ở Lab 5). Đảm bảo client **chỉ tạo một lần** và được **inject** vào đây.

---

## 🌐 3) API REST: đăng ký/huỷ đăng ký & xem dữ liệu gần nhất
`src/main/java/com/example/iotbackend/controller/SubscriptionController.java`
```java
package com.example.iotbackend.controller;

import com.example.iotbackend.mqtt.MessageStore;
import com.example.iotbackend.mqtt.SubscriptionManager;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;
import java.util.Set;

@RestController
@RequestMapping("/api/mqtt")
public class SubscriptionController {
  private final SubscriptionManager subMgr;
  private final MessageStore store;

  public SubscriptionController(SubscriptionManager subMgr, MessageStore store) {
    this.subMgr = subMgr;
    this.store = store;
  }

  // Đăng ký topic động
  @PostMapping("/subscribe")
  public ResponseEntity<?> subscribe(@RequestBody Map<String, String> body) throws MqttException {
    String topic = body.get("topic");
    subMgr.subscribe(topic);
    return ResponseEntity.ok(Map.of("subscribed", topic));
  }

  // Huỷ đăng ký topic
  @PostMapping("/unsubscribe")
  public ResponseEntity<?> unsubscribe(@RequestBody Map<String, String> body) throws MqttException {
    String topic = body.get("topic");
    subMgr.unsubscribe(topic);
    return ResponseEntity.ok(Map.of("unsubscribed", topic));
  }

  // Liệt kê topic đang subscribe
  @GetMapping("/subscriptions")
  public Set<String> current() {
    return subMgr.list();
  }

  // Xem N message gần nhất theo topic (REST polling)
  @GetMapping("/recent")
  public List<String> recent(@RequestParam String topic, @RequestParam(defaultValue = "50") int limit) {
    return store.recent(topic, limit);
  }
}
```

**Thử nhanh bằng cURL:**
```bash
# 1) Đăng ký một topic bất kỳ (ví dụ telemetry của device 1)
curl -X POST http://localhost:8080/api/mqtt/subscribe \
  -H "Content-Type: application/json" \
  -d '{"topic":"iot/device/1/telemetry"}'

# 2) Xem 10 message gần nhất
curl "http://localhost:8080/api/mqtt/recent?topic=iot/device/1/telemetry&limit=10"

# 3) Huỷ đăng ký
curl -X POST http://localhost:8080/api/mqtt/unsubscribe \
  -H "Content-Type: application/json" \
  -d '{"topic":"iot/device/1/telemetry"}'
```

---

## 🔴 4) Tuỳ chọn A — Server-Sent Events (SSE) cho giám sát realtime (đơn giản)
Nếu muốn **push realtime** nhưng vẫn giữ **đơn giản**, dùng **SSE** (1 chiều server→client).

### 4.1 Thêm endpoint SSE
`src/main/java/com/example/iotbackend/controller/StreamController.java`
```java
package com.example.iotbackend.controller;

import com.example.iotbackend.mqtt.MessageStore;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import reactor.core.publisher.Flux;

import java.time.Duration;

@RestController
public class StreamController {

  private final MessageStore store;

  public StreamController(MessageStore store) {
    this.store = store;
  }

  // Client sẽ nhận dữ liệu dạng text/event-stream mỗi 1s
  @GetMapping(value = "/api/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
  public Flux<String> stream(@RequestParam String topic) {
    return Flux.interval(Duration.ofSeconds(1))
        .map(tick -> String.join("\n", store.recent(topic, 1))); // gửi message mới nhất nếu có
  }
}
```
> **Yêu cầu:** Thêm dependency `spring-boot-starter-webflux` nếu dùng `Flux`:
```xml
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-webflux</artifactId>
</dependency>
```

### 4.2 Thử bằng cURL (SSE)
```bash
curl -N "http://localhost:8080/api/stream?topic=iot/device/1/telemetry"
```
`-N` giữ kết nối mở, bạn sẽ thấy message mới được đẩy xuống theo thời gian.

---

## 🟢 5) Tuỳ chọn B — WebSocket (STOMP) cho realtime (mạnh hơn)
Nếu muốn **2 chiều** (client gửi/nhận), dùng **WebSocket + STOMP**.

### 5.1 Dependency
```xml
<dependency>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-starter-websocket</artifactId>
</dependency>
```

### 5.2 Config WebSocket
`src/main/java/com/example/iotbackend/ws/WebSocketConfig.java`
```java
package com.example.iotbackend.ws;

import org.springframework.context.annotation.Configuration;
import org.springframework.messaging.simp.config.MessageBrokerRegistry;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;
import org.springframework.web.socket.config.annotation.StompEndpointRegistry;
import org.springframework.web.socket.config.annotation.WebSocketMessageBrokerConfigurer;

@Configuration
@EnableWebSocketMessageBroker
public class WebSocketConfig implements WebSocketMessageBrokerConfigurer {

  @Override
  public void configureMessageBroker(MessageBrokerRegistry config) {
    config.enableSimpleBroker("/topic"); // nơi client subscribe
    config.setApplicationDestinationPrefixes("/app"); // nơi client send
  }

  @Override
  public void registerStompEndpoints(StompEndpointRegistry registry) {
    registry.addEndpoint("/ws").setAllowedOriginPatterns("*").withSockJS();
  }
}
```

### 5.3 Bridge dữ liệu MQTT → WebSocket
Khi nhận message MQTT, ngoài lưu vào `MessageStore`, ta **publish** lên `/topic/stream` cho client.

`src/main/java/com/example/iotbackend/mqtt/SubscriptionManager.java` (bổ sung):
```java
// ...
import org.springframework.messaging.simp.SimpMessagingTemplate;
// ...

public class SubscriptionManager {
  // ...
  private final SimpMessagingTemplate messagingTemplate;

  public SubscriptionManager(MqttClient client, MqttConfig cfg, MessageStore messageStore, SimpMessagingTemplate messagingTemplate) throws MqttException {
    this.client = client;
    this.cfg = cfg;
    this.messageStore = messageStore;
    this.messagingTemplate = messagingTemplate;
    // ...
  }

  // trong messageArrived:
  @Override public void messageArrived(String topic, MqttMessage message) {
    String payload = new String(message.getPayload());
    log.info("[SUB] {} -> {}", topic, payload);
    messageStore.append(topic, payload);
    messagingTemplate.convertAndSend("/topic/stream", "{\"topic\":\"" + topic + "\",\"data\":" + payload + "}");
  }
}
```

### 5.4 Client HTML test nhanh
`src/main/resources/static/ws-test.html`
```html
<!DOCTYPE html>
<html>
<head><meta charset="utf-8"><title>WS Test</title></head>
<body>
  <h3>Sub /topic/stream</h3>
  <pre id="out"></pre>
  <script src="https://cdn.jsdelivr.net/npm/sockjs-client@1/dist/sockjs.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/stompjs@2.3.3/lib/stomp.min.js"></script>
  <script>
    const out = document.getElementById('out');
    const sock = new SockJS('/ws');
    const client = Stomp.over(sock);
    client.connect({}, () => {
      client.subscribe('/topic/stream', msg => {
        out.textContent += msg.body + '\n';
      });
    });
  </script>
</body>
</html>
```

Mở trình duyệt: `http://localhost:8080/ws-test.html` → xem dữ liệu realtime.

---

## ▶️ 6) Kiểm thử end-to-end
1. **Chạy backend**: `./mvnw spring-boot:run`  
2. **Đăng ký topic**:
   ```bash
   curl -X POST http://localhost:8080/api/mqtt/subscribe \
     -H "Content-Type: application/json" \
     -d '{"topic":"iot/device/1/telemetry"}'
   ```
3. **Publish dữ liệu** (Python hoặc mosquitto_pub):
   ```bash
   mosquitto_pub -h localhost -p 1883 -t iot/device/1/telemetry -m '{"temp": 25.7, "hum": 50.2}' -q 1
   ```
4. **Xem dữ liệu**:
   - REST: `GET /api/mqtt/recent?topic=iot/device/1/telemetry&limit=5`  
   - SSE: `curl -N "http://localhost:8080/api/stream?topic=iot/device/1/telemetry"`  
   - WebSocket: mở `ws-test.html`

---

## 🛠️ 7) Lỗi thường gặp & khắc phục
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| Subscribe báo lỗi | client chưa connect hoặc trùng clientId | Đảm bảo `MqttClient` được tạo 1 lần & đã connect |
| Không thấy dữ liệu ở REST/WebSocket | sai topic hoặc chưa publish | Kiểm tra đúng topic, dùng `mosquitto_pub` test |
| SSE không hiển thị gì | Không có message mới | Gửi thêm message hoặc giảm chu kỳ `Flux.interval` |
| WebSocket 403/404 | Chưa cấu hình endpoint hoặc URL sai | Kiểm tra `/ws` và `/topic/stream` |
| OOM do lưu nhiều message | store giữ quá nhiều | Giảm `MAX_PER_TOPIC` hoặc chuyển sang Redis |

---

## 🏁 Kết quả cuối cùng
- API **đăng ký/huỷ đăng ký** topic động chạy tốt.  
- Có thể **giám sát dữ liệu** bằng REST (polling) **hoặc** WebSocket/SSE (realtime).  
- Sẵn sàng tích hợp vào dashboard quan sát thiết bị.

---

## 📚 Bài tập củng cố
1) Thêm `GET /api/mqtt/topics` trả về **toàn bộ** topic đã từng nhận (lấy từ `MessageStore#topics`).  
2) Thêm bộ lọc: chỉ forward WebSocket khi `temp > ngưỡng`.  
3) Triển khai **Redis** thay cho in-memory để lưu message tạm (gợi ý: `Spring Data Redis`).  
