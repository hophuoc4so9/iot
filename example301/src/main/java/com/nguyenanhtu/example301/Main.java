package com.nguyenanhtu.example301;

import java.io.IOException;
import java.util.UUID;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttClient;

public class Main {
    public static void main(String[] args) {
        String mqttBroker = "tcp://localhost:1883";
        String mqttTopic = "/test/topic";
        String username = "IoTClient";
        String password = "IoTPass";
        String testMsg = "Hi from the IoT application";
        int qos = 1;

        try {
            // Tạo MQTT client
            String runtimeClientId = "mqttClient-" + UUID.randomUUID();
            MqttClient mqttClient = new MqttClient(mqttBroker, runtimeClientId);
            
            // Cấu hình thông tin đăng nhập
            MqttConnectOptions mqttOptions = new MqttConnectOptions();
            mqttOptions.setUserName(username);
            mqttOptions.setPassword(password.toCharArray());
            mqttOptions.setAutomaticReconnect(true);
            mqttOptions.setCleanSession(true);

            // Kết nối broker
            mqttClient.connect(mqttOptions);
            System.out.println("Connected to broker: " + mqttBroker);

            // Đăng ký callback xử lý khi có tin nhắn, mất kết nối, hoàn tất gửi
            mqttClient.setCallback(new MqttCallback() {
                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    byte[] payload = message != null ? message.getPayload() : null;
                    if (payload == null || payload.length == 0) {
                        System.out.println("Received empty message on topic: " + topic);
                    } else {
                        System.out.println("Received message on topic [" + topic + "]: " + new String(payload));
                    }
                }

                @Override
                public void connectionLost(Throwable cause) {
                    System.out.println("Connection lost: " + cause.getMessage());
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    System.out.println("Message delivery complete: " + token.isComplete());
                }
            });

            // Đăng ký topic
            mqttClient.subscribe(mqttTopic, qos);
            System.out.println("Subscribed to topic: " + mqttTopic);

            // Gửi một tin nhắn thử nghiệm
            MqttMessage mqttMsg = new MqttMessage(testMsg.getBytes());
            mqttMsg.setQos(qos);
            mqttClient.publish(mqttTopic, mqttMsg);
            System.out.println("Published message: " + testMsg);

            System.out.println("Press Enter to disconnect...");
            System.in.read();

            // Ngắt kết nối
            mqttClient.disconnect();
            mqttClient.close();
            System.out.println("Disconnected.");

        } catch (MqttException e) {
            e.printStackTrace();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
