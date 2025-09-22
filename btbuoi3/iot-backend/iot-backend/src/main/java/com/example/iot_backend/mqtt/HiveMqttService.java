package com.example.iot_backend.mqtt;

import com.example.iot_backend.config.MqttConfig;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.hivemq.client.mqtt.MqttClient;
import com.hivemq.client.mqtt.datatypes.MqttQos;
import com.hivemq.client.mqtt.mqtt3.Mqtt3AsyncClient;
import jakarta.annotation.PreDestroy;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Bean;

import java.nio.charset.StandardCharsets;
import java.util.Optional;

// @Service  // Disabled to avoid bean conflict with PahoMqttService
public class HiveMqttService {
  private static final Logger log = LoggerFactory.getLogger(HiveMqttService.class);
  private final MqttConfig cfg;
  private final ObjectMapper objectMapper;
  private Mqtt3AsyncClient client;

  public HiveMqttService(MqttConfig cfg) {
    this.cfg = cfg;
    this.objectMapper = new ObjectMapper();
  }

  @Bean
  public ApplicationRunner mqttRunner() {
    return args -> {
      connectAndSubscribe();
    };
  }

  private void connectAndSubscribe() {
    client = MqttClient.builder()
        .useMqttVersion3()
        .identifier(cfg.getClientId())
        .serverHost(cfg.getHost())
        .serverPort(cfg.getPort())
        .buildAsync();

    var connectBuilder = client.connectWith()
        .keepAlive(cfg.getKeepAlive());

    // Add authentication if username is provided
    if (!Optional.ofNullable(cfg.getUsername()).orElse("").isBlank()) {
      connectBuilder = connectBuilder.simpleAuth()
          .username(cfg.getUsername())
          .password(StandardCharsets.UTF_8.encode(Optional.ofNullable(cfg.getPassword()).orElse("")))
          .applySimpleAuth();
    }

    connectBuilder.send()
        .whenComplete((ack, ex) -> {
          if (ex != null) {
            log.error("MQTT connect failed: {}", ex.getMessage(), ex);
            return;
          }
          log.info("MQTT connected as clientId={}", cfg.getClientId());

          // Subscribe to temperature topic
          subscribeToTopic(cfg.getTopicTemp());
          
          // Subscribe to humidity topic
          subscribeToTopic(cfg.getTopicHum());
        });
  }

  private void subscribeToTopic(String topic) {
    client.subscribeWith()
        .topicFilter(topic)
        .qos(MqttQos.fromCode(cfg.getQos()))
        .callback(publish -> {
          String payload = new String(publish.getPayloadAsBytes(), StandardCharsets.UTF_8);
          String receivedTopic = publish.getTopic().toString();
          
          log.info("[HiveMQ] Received topic='{}' payload={}", receivedTopic, payload);
          
          // Parse JSON and check for temperature alerts
          processMessage(receivedTopic, payload);
        })
        .send()
        .whenComplete((subAck, subEx) -> {
          if (subEx != null) {
            log.error("MQTT subscribe failed for topic {}: {}", topic, subEx.getMessage(), subEx);
          } else {
            log.info("Subscribed to topic {}", topic);
          }
        });
  }

  private void processMessage(String topic, String payload) {
    try {
      JsonNode jsonNode = objectMapper.readTree(payload);
      
      // Check for temperature alerts if this is a temperature topic
      if (topic.equals(cfg.getTopicTemp()) && jsonNode.has("temp")) {
        double temperature = jsonNode.get("temp").asDouble();
        if (temperature > 30.0) {
          log.warn("🌡️ TEMPERATURE ALERT: {}°C is above 30°C threshold! Topic: {}", 
                   temperature, topic);
        }
      }
      
      // Log humidity data if this is humidity topic
      if (topic.equals(cfg.getTopicHum()) && jsonNode.has("hum")) {
        double humidity = jsonNode.get("hum").asDouble();
        log.info("💧 Humidity reading: {}% from topic: {}", humidity, topic);
      }
      
    } catch (Exception e) {
      log.error("Failed to parse JSON payload from topic {}: {}", topic, e.getMessage());
    }
  }

  // Method to publish messages (will be used by the controller)
  public void publishMessage(String topic, String message) {
    if (client != null && client.getState().isConnected()) {
      client.publishWith()
          .topic(topic)
          .qos(MqttQos.fromCode(cfg.getQos()))
          .payload(message.getBytes(StandardCharsets.UTF_8))
          .send()
          .whenComplete((result, ex) -> {
            if (ex != null) {
              log.error("Failed to publish message to topic {}: {}", topic, ex.getMessage());
            } else {
              log.info("Successfully published message to topic {}: {}", topic, message);
            }
          });
    } else {
      log.error("Cannot publish message - MQTT client is not connected");
    }
  }

  @PreDestroy
  public void shutdown() {
    if (client != null) {
      client.disconnect();
      log.info("MQTT disconnected");
    }
  }
}
