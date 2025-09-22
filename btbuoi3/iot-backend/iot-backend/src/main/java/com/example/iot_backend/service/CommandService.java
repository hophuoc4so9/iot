package com.example.iot_backend.service;

import com.example.iot_backend.mqtt.PahoMqttService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

@Service
public class CommandService {
  private static final Logger log = LoggerFactory.getLogger(CommandService.class);
  private final PahoMqttService mqttService;

  public CommandService(PahoMqttService mqttService) {
    this.mqttService = mqttService;
  }

  public void sendCommand(Long deviceId, String action) {
    String topic = "iot/device/" + deviceId + "/command";
    String payload = String.format("{\"action\":\"%s\"}", action);

    mqttService.publishMessage(topic, payload);
    log.info("Published command={} to topic={}", action, topic);
  }

  public void sendCommandWithDuration(Long deviceId, String action, Integer duration) {
    String topic = "iot/device/" + deviceId + "/command";
    String payload;
    if (duration != null) {
      payload = String.format("{\"action\":\"%s\",\"duration\":%d}", action, duration);
    } else {
      payload = String.format("{\"action\":\"%s\"}", action);
    }

    mqttService.publishMessage(topic, payload);
    log.info("Published command={} with duration={} to topic={}", action, duration, topic);
  }

  public void sendCustomCommand(Long deviceId, String customPayload) {
    String topic = "iot/device/" + deviceId + "/command";
    
    mqttService.publishMessage(topic, customPayload);
    log.info("Published custom command to topic={}, payload={}", topic, customPayload);
  }
}