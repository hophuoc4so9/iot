package com.nguyenanhtu.example301;
import java.io.IOException;
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
MqttClient mqttClient = new MqttClient(mqttBroker, "mqttClient");
MqttConnectOptions mqttOptions = new MqttConnectOptions();
mqttOptions.setUserName(username);
mqttOptions.setPassword(password.toCharArray());
mqttClient.connect(mqttOptions);
if (mqttClient.isConnected()) {
mqttClient.setCallback(new MqttCallback() {
@Override
public void messageArrived(String topic, MqttMessage message) throws ExceSystem.out.println("Received message: " + new String(message.getPaylo}
@Override
public void connectionLost(Throwable cause) {
System.out.println("Connection is lost: " + cause.getMessage());
}
@Override
public void deliveryComplete(IMqttDeliveryToken token) {
System.out.println("Message publish is complete: " + token.isComplete}
});
mqttClient.subscribe(mqttTopic, qos);
MqttMessage mqttMsg = new MqttMessage(testMsg.getBytes());
mqttMsg.setQos(qos);
mqttClient.publish(mqttTopic, mqttMsg);
}
System.out.println("Press Enter to disconnect");
System.in.read();
mqttClient.disconnect();
mqttClient.close();
} catch (MqttException e) {
e.printStackTrace();
} catch (IOException e) {
throw new RuntimeException(e);
}
}
}