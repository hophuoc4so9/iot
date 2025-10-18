package btbuoi6.btbuoi6.controller;

import btbuoi6.btbuoi6.model.Telemetry;
import btbuoi6.btbuoi6.repository.TelemetryRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/telemetry")
@CrossOrigin(origins = "*")
public class TelemetryController {
    
    @Autowired
    private TelemetryRepository telemetryRepository;

    @GetMapping("/{deviceId}")
    public List<Telemetry> getByDevice(@PathVariable Long deviceId) {
        return telemetryRepository.findByDeviceId(deviceId);
    }
    
    @GetMapping
    public List<Telemetry> getAllTelemetry() {
        return telemetryRepository.findAll();
    }
}
