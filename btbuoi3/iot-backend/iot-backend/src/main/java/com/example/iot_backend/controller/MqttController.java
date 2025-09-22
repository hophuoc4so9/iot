package com.example.iot_backend.controller;

import com.example.iot_backend.config.MqttConfig;
import com.example.iot_backend.mqtt.PahoMqttService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/mqtt")
public class MqttController {
  
  private static final Logger log = LoggerFactory.getLogger(MqttController.class);
  private final PahoMqttService mqttService;
  private final MqttConfig mqttConfig;

  public MqttController(PahoMqttService mqttService, MqttConfig mqttConfig) {
    this.mqttService = mqttService;
    this.mqttConfig = mqttConfig;
  }

  @PostMapping("/publish")
  public ResponseEntity<?> publishMessage(@RequestBody PublishRequest request) {
    try {
      // Use the configured command topic if no topic is specified
      String topic = request.getTopic() != null ? request.getTopic() : mqttConfig.getTopicCmd();
      
      if (request.getMessage() == null || request.getMessage().trim().isEmpty()) {
        return ResponseEntity.badRequest()
            .body(Map.of("error", "Message cannot be empty"));
      }

      mqttService.publishMessage(topic, request.getMessage());
      
      log.info("API: Published message to topic '{}': {}", topic, request.getMessage());
      
      return ResponseEntity.ok(Map.of(
          "status", "success",
          "topic", topic,
          "message", request.getMessage(),
          "timestamp", System.currentTimeMillis()
      ));
      
    } catch (Exception e) {
      log.error("Error publishing MQTT message: {}", e.getMessage());
      return ResponseEntity.internalServerError()
          .body(Map.of("error", "Failed to publish message: " + e.getMessage()));
    }
  }

  // DTO for the publish request
  public static class PublishRequest {
    private String topic;
    private String message;

    public String getTopic() {
      return topic;
    }

    public void setTopic(String topic) {
      this.topic = topic;
    }

    public String getMessage() {
      return message;
    }

    public void setMessage(String message) {
      this.message = message;
    }
  }
}