package com.example.iot_backend.mqtt;

import com.example.iot_backend.config.MqttConfig;
import com.example.iot_backend.service.TelemetryService;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.hivemq.client.mqtt.MqttClient;
import com.hivemq.client.mqtt.datatypes.MqttQos;
import com.hivemq.client.mqtt.mqtt3.Mqtt3AsyncClient;
import com.hivemq.client.mqtt.mqtt3.message.publish.Mqtt3Publish;
import jakarta.annotation.PreDestroy;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

import java.nio.charset.StandardCharsets;
import java.util.Optional;

@Service
public class PahoMqttService {
  private static final Logger log = LoggerFactory.getLogger(PahoMqttService.class);
  private final MqttConfig cfg;
  private final TelemetryService telemetryService;
  private final ObjectMapper objectMapper;
  private Mqtt3AsyncClient client;

  public PahoMqttService(MqttConfig cfg, TelemetryService telemetryService) {
    this.cfg = cfg;
    this.telemetryService = telemetryService;
    this.objectMapper = new ObjectMapper();
  }

  @Bean
  public ApplicationRunner mqttRunner() {
    return args -> connectAndSubscribe();
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
          log.info("Connected to MQTT broker {}:{}", cfg.getHost(), cfg.getPort());

          // Subscribe to device telemetry (wildcard pattern)
          subscribeToDeviceTelemetry();
          
          // Subscribe to temperature topic if configured
          if (cfg.getTopicTemp() != null && !cfg.getTopicTemp().isBlank()) {
            subscribeToTopic(cfg.getTopicTemp());
          }
          
          // Subscribe to humidity topic if configured
          if (cfg.getTopicHum() != null && !cfg.getTopicHum().isBlank()) {
            subscribeToTopic(cfg.getTopicHum());
          }
        });
  }

  private void subscribeToDeviceTelemetry() {
    String topic = "iot/device/+/telemetry";
    client.subscribeWith()
        .topicFilter(topic)
        .qos(MqttQos.fromCode(cfg.getQos()))
        .callback(this::messageArrived)
        .send()
        .whenComplete((subAck, subEx) -> {
          if (subEx != null) {
            log.error("MQTT subscribe failed for topic {}: {}", topic, subEx.getMessage(), subEx);
          } else {
            log.info("Subscribed to device telemetry: {}", topic);
          }
        });
  }

  private void subscribeToTopic(String topic) {
    client.subscribeWith()
        .topicFilter(topic)
        .qos(MqttQos.fromCode(cfg.getQos()))
        .callback(this::messageArrived)
        .send()
        .whenComplete((subAck, subEx) -> {
          if (subEx != null) {
            log.error("MQTT subscribe failed for topic {}: {}", topic, subEx.getMessage(), subEx);
          } else {
            log.info("Subscribed to topic: {}", topic);
          }
        });
  }

  private void messageArrived(Mqtt3Publish publish) {
    String topic = publish.getTopic().toString();
    String payload = new String(publish.getPayloadAsBytes(), StandardCharsets.UTF_8);
    log.info("Received topic={} payload={}", topic, payload);

    // Handle device telemetry messages
    if (topic.matches("iot/device/.+/telemetry")) {
      handleDeviceTelemetry(topic, payload);
    } else {
      // Handle other topic messages (temperature/humidity alerts)
      processMessage(topic, payload);
    }
  }

  private void handleDeviceTelemetry(String topic, String payload) {
    try {
      // Extract deviceId from topic: iot/device/{id}/telemetry
      String[] parts = topic.split("/");
      Long deviceId = Long.parseLong(parts[2]);
      telemetryService.saveTelemetry(deviceId, payload);
    } catch (Exception e) {
      log.error("Failed to save telemetry for topic {}: {}", topic, e.getMessage(), e);
    }
  }

  private void processMessage(String topic, String payload) {
    try {
      JsonNode jsonNode = objectMapper.readTree(payload);
      
      // Check for temperature alerts if this is a temperature topic
      if (cfg.getTopicTemp() != null && topic.equals(cfg.getTopicTemp()) && jsonNode.has("temp")) {
        double temperature = jsonNode.get("temp").asDouble();
        if (temperature > 30.0) {
          log.warn("TEMPERATURE ALERT: {}°C is above 30°C threshold! Topic: {}", 
                   temperature, topic);
        }
      }
      
      // Log humidity data if this is humidity topic
      if (cfg.getTopicHum() != null && topic.equals(cfg.getTopicHum()) && jsonNode.has("hum")) {
        double humidity = jsonNode.get("hum").asDouble();
        log.info("Humidity reading: {}% from topic: {}", humidity, topic);
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
  public void disconnect() {
    if (client != null && client.getState().isConnected()) {
      client.disconnect()
          .whenComplete((ack, ex) -> {
            if (ex != null) {
              log.error("MQTT disconnect failed: {}", ex.getMessage(), ex);
            } else {
              log.info("MQTT disconnected");
            }
          });
    }
  }
}