package backend_iot.backend_iot;

import backend_iot.backend_iot.model.DeviceState;
import backend_iot.backend_iot.model.SensorData;
import backend_iot.backend_iot.repository.DeviceStateRepository;
import backend_iot.backend_iot.repository.SensorDataRepository;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.annotation.PostConstruct;
import jakarta.annotation.PreDestroy;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;

@Service
public class MqttService {
    private final Logger log = LoggerFactory.getLogger(MqttService.class);

    private final SensorDataRepository repository;
    private final DeviceStateRepository deviceStateRepository;
    private final ObjectMapper objectMapper;

    public MqttService(SensorDataRepository repository, DeviceStateRepository deviceStateRepository) {
        this.repository = repository;
        this.deviceStateRepository = deviceStateRepository;
        this.objectMapper = new ObjectMapper();
    }

    @Value("${mqtt.broker:tcp://localhost:1883}")
    private String brokerUrl;

    @Value("${mqtt.clientId:backend-iot-client}")
    private String clientId;

    @Value("${mqtt.subscribeTopic:esp32/#}")
    private String subscribeTopic;

    private MqttClient client;

    @PostConstruct
    public void init() {
        try {
            client = new MqttClient(brokerUrl, clientId, new MemoryPersistence());
            MqttConnectOptions opts = new MqttConnectOptions();
            opts.setAutomaticReconnect(true);
            opts.setCleanSession(true);
            client.connect(opts);
            log.info("Connected to MQTT broker {}", brokerUrl);

            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    log.warn("MQTT connection lost", cause);
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
                    log.info("MQTT message arrived [{}] {}", topic, payload);
                    try {
                        // Save raw sensor data
                        SensorData data = new SensorData(topic, payload, LocalDateTime.now());
                        repository.save(data);
                        log.info("Saved sensor data id={}", data.getId());

                        // Parse and save device state if it's from device/state topic
                        if (topic.contains("/device/state")) {
                            try {
                                JsonNode json = objectMapper.readTree(payload);
                                String deviceId = extractDeviceId(topic);
                                
                                if (json.has("led_rgb")) {
                                    JsonNode rgbArray = json.get("led_rgb");
                                    if (rgbArray.isArray() && rgbArray.size() >= 3) {
                                        int r = rgbArray.get(0).asInt();
                                        int g = rgbArray.get(1).asInt();
                                        int b = rgbArray.get(2).asInt();
                                        
                                        DeviceState deviceState = new DeviceState(
                                            deviceId, r, g, b, payload, LocalDateTime.now()
                                        );
                                        deviceStateRepository.save(deviceState);
                                        log.info("Saved device state for device={}, RGB=[{},{},{}]", deviceId, r, g, b);
                                    }
                                }
                            } catch (Exception e) {
                                log.warn("Failed to parse device state JSON: {}", e.getMessage());
                            }
                        }
                    } catch (Exception ex) {
                        log.error("Failed to save sensor data", ex);
                    }
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    // no-op
                }
            });

            client.subscribe(subscribeTopic, 1);
            log.info("Subscribed to topic {}", subscribeTopic);
        } catch (MqttException e) {
            log.error("Failed to initialize MQTT client", e);
        }
    }

    public void publish(String topic, String payload, int qos, boolean retained) throws MqttException {
        if (client == null || !client.isConnected()) {
            throw new MqttException(new Throwable("MQTT client is not connected"));
        }
        MqttMessage message = new MqttMessage(payload.getBytes(StandardCharsets.UTF_8));
        message.setQos(qos);
        message.setRetained(retained);
        client.publish(topic, message);
        log.info("Published to {}: {}", topic, payload);
    }

    private String extractDeviceId(String topic) {
        // Extract device ID from topic like "iot/demo/device/state" -> "demo"
        String[] parts = topic.split("/");
        if (parts.length >= 2) {
            return parts[1]; // return namespace as device ID
        }
        return "unknown";
    }

    @PreDestroy
    public void shutdown() {
        try {
            if (client != null && client.isConnected()) {
                client.disconnect();
                client.close();
            }
        } catch (MqttException e) {
            log.warn("Error while closing MQTT client", e);
        }
    }
}
