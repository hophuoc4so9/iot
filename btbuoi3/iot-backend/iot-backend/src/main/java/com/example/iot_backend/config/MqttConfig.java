package com.example.iot_backend.config;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
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
  // thêm topic Hum và topic Cmd
  @Value("${mqtt.topicHum}") private String topicHum;
  @Value("${mqtt.topicCmd}") private String topicCmd;
  @Value("${mqtt.qos:1}") private int qos;
  @Value("${mqtt.keepAlive:30}") private int keepAlive;

  public String getHost() { return host; }
  public int getPort() { return port; }
  public String getUsername() { return username; }
  public String getPassword() { return password; }
  public String getClientId() { return clientId; }
  public String getTopicTemp() { return topicTemp; }
  public String getTopicHum() { return topicHum; }
  public String getTopicCmd() { return topicCmd; }
  public int getQos() { return qos; }
  public int getKeepAlive() { return keepAlive; }

  @Bean
  public MqttClient mqttClient() throws MqttException {
    String brokerUrl = "tcp://" + host + ":" + port;
    MqttClient client = new MqttClient(brokerUrl, clientId + "_paho");
    
    MqttConnectOptions options = new MqttConnectOptions();
    options.setKeepAliveInterval(keepAlive);
    options.setCleanSession(true);
    
    if (username != null && !username.trim().isEmpty()) {
      options.setUserName(username);
      if (password != null) {
        options.setPassword(password.toCharArray());
      }
    }
    
    client.connect(options);
    return client;
  }
}