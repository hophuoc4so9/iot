package btbuoi6.btbuoi6.controller;

import btbuoi6.btbuoi6.model.Device;
import btbuoi6.btbuoi6.repository.DeviceRepository;
import btbuoi6.btbuoi6.service.MqttPublisherService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/devices")
@CrossOrigin(origins = "*")
public class DeviceController {
    
    @Autowired
    private DeviceRepository deviceRepository;
    
    @Autowired
    private MqttPublisherService mqttPublisherService;
    
    @Autowired
    private MqttPahoMessageDrivenChannelAdapter mqttAdapter;

    @GetMapping
    public List<Device> getAllDevices() {
        return deviceRepository.findAll();
    }

    @PostMapping
    public Device createDevice(@RequestBody Device device) {
        // Subscribe to the device's topic
        mqttAdapter.addTopic(device.getTopic(), 1);
        return deviceRepository.save(device);
    }

    @PostMapping("/{id}/control")
    public String controlDevice(@PathVariable Long id, @RequestBody String payload) {
        Device device = deviceRepository.findById(id).orElse(null);
        if (device != null) {
            mqttPublisherService.publish(device.getTopic(), payload);
            return "Published to " + device.getTopic();
        }
        return "Device not found";
    }
}
