package com.example.iot_backend.mqtt;

import com.example.iot_backend.config.MqttConfig;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.eclipse.paho.client.mqttv3.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Component;

import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

@Component
public class SubscriptionManager {
  private static final Logger log = LoggerFactory.getLogger(SubscriptionManager.class);
  private static final double TEMPERATURE_THRESHOLD = 25.0; // Ngưỡng nhiệt độ cho WebSocket forwarding
  
  private final MqttClient client;
  private final MqttConfig cfg;
  private final MessageStore messageStore;
  private final RedisMessageStore redisMessageStore; // Redis store for Lab 6 bài tập 3
  private final SimpMessagingTemplate messagingTemplate;
  private final ObjectMapper objectMapper;

  private final Set<String> currentTopics = ConcurrentHashMap.newKeySet();

  public SubscriptionManager(MqttClient client, MqttConfig cfg, MessageStore messageStore, RedisMessageStore redisMessageStore, SimpMessagingTemplate messagingTemplate) throws MqttException {
    this.client = client;
    this.cfg = cfg;
    this.messageStore = messageStore;
    this.redisMessageStore = redisMessageStore;
    this.messagingTemplate = messagingTemplate;
    this.objectMapper = new ObjectMapper();

    this.client.setCallback(new MqttCallback() {
      @Override public void connectionLost(Throwable cause) {
        log.warn("MQTT lost: {}", cause.getMessage());
      }
      @Override public void messageArrived(String topic, MqttMessage message) {
        String payload = new String(message.getPayload());
        log.info("[SUB] {} -> {}", topic, payload);
        
        // Lưu vào cả in-memory và Redis store (Lab 6 bài tập 3)
        messageStore.append(topic, payload);
        redisMessageStore.append(topic, payload);
        
        // Bộ lọc: chỉ forward WebSocket khi temp > ngưỡng (Lab 6 bài tập 2)
        if (shouldForwardToWebSocket(payload)) {
          messagingTemplate.convertAndSend("/topic/stream", "{\"topic\":\"" + topic + "\",\"data\":" + payload + "}");
          log.info("Forwarded to WebSocket: {} -> {}", topic, payload);
        } else {
          log.debug("Message filtered out (temp <= {}): {}", TEMPERATURE_THRESHOLD, payload);
        }
      }
      @Override public void deliveryComplete(IMqttDeliveryToken token) {}
    });

    // The client is already connected from the bean configuration
    if (!client.isConnected()) {
      log.warn("MQTT client is not connected, attempting to reconnect...");
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

  /**
   * Kiểm tra xem có nên forward message lên WebSocket hay không
   * Chỉ forward khi nhiệt độ > ngưỡng (Lab 6 bài tập 2)
   */
  private boolean shouldForwardToWebSocket(String payload) {
    try {
      JsonNode jsonNode = objectMapper.readTree(payload);
      if (jsonNode.has("temp")) {
        double temperature = jsonNode.get("temp").asDouble();
        return temperature > TEMPERATURE_THRESHOLD;
      }
      // Nếu không có trường temp, vẫn forward (có thể là dữ liệu khác)
      return true;
    } catch (Exception e) {
      log.warn("Failed to parse JSON payload for temperature filtering: {}", payload, e);
      // Nếu không parse được JSON, vẫn forward để không mất dữ liệu
      return true;
    }
  }
}