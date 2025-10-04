package com.example.iot_backend.controller;


import com.example.iot_backend.mqtt.MessageStore;
import com.example.iot_backend.mqtt.RedisMessageStore;
import com.example.iot_backend.mqtt.SubscriptionManager;
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
  private final MessageStore store; // In-memory store for backward compatibility
  private final RedisMessageStore redisStore; // Redis store for Lab 6 bài tập 3

  
  public SubscriptionController(SubscriptionManager subMgr, MessageStore store, RedisMessageStore redisStore) {
    this.subMgr = subMgr;
    this.store = store;
    this.redisStore = redisStore;
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

  // Xem N message gần nhất theo topic (REST polling) - Redis version
  @GetMapping("/recent")
  public List<String> recent(@RequestParam String topic, @RequestParam(defaultValue = "50") int limit) {
    return store.recent(topic, limit);
  }

  // Xem N message gần nhất theo topic từ in-memory store (backward compatibility)
  @GetMapping("/recent/memory")
  public List<String> recentFromMemory(@RequestParam String topic, @RequestParam(defaultValue = "50") int limit) {
    return store.recent(topic, limit);
  }

  // Trả về toàn bộ topic đã từng nhận từ Redis (Lab 6 bài tập 1)
  @GetMapping("/topics")
  public Set<String> getAllTopics() {
    return store.topics();
  }

  // Trả về toàn bộ topic từ in-memory store (backward compatibility)
  @GetMapping("/topics/memory")
  public Set<String> getAllTopicsFromMemory() {
    return store.topics();
  }

  // Clear Redis storage
  @DeleteMapping("/clear")
  public ResponseEntity<?> clearStorage(@RequestParam(required = false) String topic) {
    if (topic != null) {
      redisStore.clear(topic);
      return ResponseEntity.ok(Map.of("cleared", topic));
    } else {
      redisStore.clearAll();
      return ResponseEntity.ok(Map.of("message", "All topics cleared"));
    }
  }
}