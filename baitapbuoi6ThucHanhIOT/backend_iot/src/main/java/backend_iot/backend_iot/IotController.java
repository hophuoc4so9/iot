package backend_iot.backend_iot;

import backend_iot.backend_iot.dto.DeviceStateResponse;
import backend_iot.backend_iot.dto.LedCommandRequest;
import backend_iot.backend_iot.model.DeviceState;
import backend_iot.backend_iot.model.SensorData;
import backend_iot.backend_iot.repository.DeviceStateRepository;
import backend_iot.backend_iot.repository.SensorDataRepository;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@RestController
@RequestMapping("/api/v1")
@CrossOrigin(origins = "*")
public class IotController {
    private final Logger log = LoggerFactory.getLogger(IotController.class);
    private final MqttService mqttService;
    private final SensorDataRepository repository;
    private final DeviceStateRepository deviceStateRepository;
    private final ObjectMapper objectMapper;

    @Value("${mqtt.namespace:iot/demo}")
    private String namespace;

    public IotController(MqttService mqttService, SensorDataRepository repository, 
                        DeviceStateRepository deviceStateRepository) {
        this.mqttService = mqttService;
        this.repository = repository;
        this.deviceStateRepository = deviceStateRepository;
        this.objectMapper = new ObjectMapper();
    }

    // ===== MQTT Publish =====
    @PostMapping("/publish")
    public ResponseEntity<?> publish(@RequestParam String topic, @RequestBody String payload) {
        try {
            mqttService.publish(topic, payload, 1, false);
            return ResponseEntity.ok().body(Map.of("status", "published", "topic", topic));
        } catch (MqttException e) {
            log.error("Failed to publish", e);
            return ResponseEntity.status(500).body(Map.of("error", e.getMessage()));
        }
    }

    // ===== Device Control =====
    @PostMapping("/device/led")
    public ResponseEntity<?> controlLed(@RequestBody LedCommandRequest request) {
        try {
            String topic = namespace + "/device/cmd";
            String payload = objectMapper.writeValueAsString(request);
            mqttService.publish(topic, payload, 1, false);
            
            return ResponseEntity.ok().body(Map.of(
                "status", "command sent",
                "topic", topic,
                "command", request
            ));
        } catch (Exception e) {
            log.error("Failed to control LED", e);
            return ResponseEntity.status(500).body(Map.of("error", e.getMessage()));
        }
    }

    @PostMapping("/device/led/rgb")
    public ResponseEntity<?> setLedRgb(@RequestParam int r, @RequestParam int g, @RequestParam int b) {
        try {
            LedCommandRequest request = new LedCommandRequest(new int[]{r, g, b});
            String topic = namespace + "/device/cmd";
            String payload = objectMapper.writeValueAsString(request);
            mqttService.publish(topic, payload, 1, false);
            
            return ResponseEntity.ok().body(Map.of(
                "status", "LED color updated",
                "rgb", new int[]{r, g, b}
            ));
        } catch (Exception e) {
            log.error("Failed to set LED RGB", e);
            return ResponseEntity.status(500).body(Map.of("error", e.getMessage()));
        }
    }

    @PostMapping("/device/led/off")
    public ResponseEntity<?> turnOffLed() {
        try {
            Map<String, Object> command = new HashMap<>();
            command.put("off", true);
            String topic = namespace + "/device/cmd";
            String payload = objectMapper.writeValueAsString(command);
            mqttService.publish(topic, payload, 1, false);
            
            return ResponseEntity.ok().body(Map.of("status", "LED turned off"));
        } catch (Exception e) {
            log.error("Failed to turn off LED", e);
            return ResponseEntity.status(500).body(Map.of("error", e.getMessage()));
        }
    }

    // ===== Device State =====
    @GetMapping("/device/state")
    public ResponseEntity<?> getDeviceState(@RequestParam(defaultValue = "demo") String deviceId) {
        return deviceStateRepository.findFirstByDeviceIdOrderByTimestampDesc(deviceId)
                .map(state -> ResponseEntity.ok().body(new DeviceStateResponse(state)))
                .orElse(ResponseEntity.notFound().build());
    }

    @GetMapping("/device/state/history")
    public ResponseEntity<List<DeviceStateResponse>> getDeviceStateHistory(
            @RequestParam(defaultValue = "demo") String deviceId,
            @RequestParam(defaultValue = "100") int limit) {
        List<DeviceState> states = deviceStateRepository.findByDeviceIdOrderByTimestampDesc(deviceId);
        List<DeviceStateResponse> response = states.stream()
                .limit(limit)
                .map(DeviceStateResponse::new)
                .collect(Collectors.toList());
        return ResponseEntity.ok(response);
    }

    @GetMapping("/device/states/recent")
    public ResponseEntity<List<DeviceStateResponse>> getRecentDeviceStates() {
        List<DeviceState> states = deviceStateRepository.findTop100ByOrderByTimestampDesc();
        List<DeviceStateResponse> response = states.stream()
                .map(DeviceStateResponse::new)
                .collect(Collectors.toList());
        return ResponseEntity.ok(response);
    }

    // ===== Sensor Data =====
    @PostMapping("/sensor")
    public ResponseEntity<SensorData> receiveSensor(
            @RequestParam(required = false, defaultValue = "esp32/sensor") String topic,
            @RequestBody String payload) {
        SensorData data = new SensorData(topic, payload, LocalDateTime.now());
        SensorData saved = repository.save(data);
        return ResponseEntity.ok(saved);
    }

    @GetMapping("/sensor/recent")
    public ResponseEntity<List<SensorData>> getRecentSensorData(
            @RequestParam(defaultValue = "100") int limit) {
        List<SensorData> data = repository.findTop100ByOrderByReceivedAtDesc();
        return ResponseEntity.ok(data.stream().limit(limit).collect(Collectors.toList()));
    }

    @GetMapping("/sensor/all")
    public ResponseEntity<List<SensorData>> getAllSensorData() {
        return ResponseEntity.ok(repository.findAll());
    }

    @GetMapping("/sensor/{id}")
    public ResponseEntity<SensorData> getSensorDataById(@PathVariable Long id) {
        return repository.findById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    // ===== System Info =====
    @GetMapping("/info")
    public ResponseEntity<?> getSystemInfo() {
        Map<String, Object> info = new HashMap<>();
        info.put("namespace", namespace);
        info.put("topicDeviceCmd", namespace + "/device/cmd");
        info.put("topicDeviceState", namespace + "/device/state");
        info.put("totalSensorRecords", repository.count());
        info.put("totalDeviceStateRecords", deviceStateRepository.count());
        return ResponseEntity.ok(info);
    }
}
