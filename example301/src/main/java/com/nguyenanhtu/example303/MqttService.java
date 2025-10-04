package com.nguyenanhtu.example303;
import jakarta.annotation.PostConstruct;
import jakarta.annotation.PreDestroy;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
@Service
public class MqttService {
  @Value("${mqtt.broker}")
  private String brokerUrl;
  @Value("${mqtt.clientId}")
  private String clientId;
  @Value("${mqtt.topic}")
  private String topic;
  private MqttClient mqttClient;
  @PostConstruct
  public void connect() throws MqttException {
    mqttClient = new MqttClient(brokerUrl, clientId, null);
    MqttConnectOptions options = new MqttConnectOptions();
    options.setAutomaticReconnect(true);
    options.setCleanSession(true);
    mqttClient.connect(options);
    mqttClient.subscribe(topic, (t, msg) -> {
      System.out.println("Received from topic [" + t + "]: " + new String(msg.getPayload()));
    });
    System.out.println("Connected and subscribed to: " + topic);
  }
  public void publishMessage(String message) throws MqttException {
    MqttMessage mqttMessage = new MqttMessage(message.getBytes());
    mqttClient.publish(topic, mqttMessage);
  }
  @PreDestroy
  public void disconnect() throws MqttException {
    if (mqttClient != null && mqttClient.isConnected()) {
      mqttClient.disconnect();
    }
  }
}