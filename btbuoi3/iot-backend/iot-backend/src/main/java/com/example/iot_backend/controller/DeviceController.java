package com.example.iot_backend.controller;


import com.example.iot_backend.model.Device;
import com.example.iot_backend.model.Telemetry;
import com.example.iot_backend.service.DeviceService;
import com.example.iot_backend.service.TelemetryService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/devices")
public class DeviceController {
  private final DeviceService deviceService;
  private final TelemetryService telemetryService;

  public DeviceController(DeviceService deviceService, TelemetryService telemetryService) {
    this.deviceService = deviceService;
    this.telemetryService = telemetryService;
  }

  // GET: danh sách thiết bị
  @GetMapping
  public List<Device> getAllDevices() {
    return deviceService.findAll();
  }

  // GET: 1 thiết bị theo id
  @GetMapping("/{id}")
  public ResponseEntity<Device> getDeviceById(@PathVariable Long id) {
    return deviceService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
  }

  // POST: thêm mới thiết bị
  @PostMapping
  public Device createDevice(@RequestBody Device device) {
    return deviceService.save(device);
  }

  // PUT: cập nhật thiết bị
  @PutMapping("/{id}")
  public ResponseEntity<Device> updateDevice(@PathVariable Long id, @RequestBody Device device) {
    return deviceService.findById(id)
        .map(existing -> {
          existing.setName(device.getName());
          existing.setType(device.getType());
          existing.setStatus(device.getStatus());
          return ResponseEntity.ok(deviceService.save(existing));
        })
        .orElse(ResponseEntity.notFound().build());
  }

  // DELETE: xóa thiết bị
  @DeleteMapping("/{id}")
  public ResponseEntity<Void> deleteDevice(@PathVariable Long id) {
    if (deviceService.findById(id).isPresent()) {
      deviceService.delete(id);
      return ResponseEntity.noContent().build();
    } else {
      return ResponseEntity.notFound().build();
    }
  }

  // GET: lấy dữ liệu telemetry gần nhất của thiết bị
  @GetMapping("/{id}/telemetry")
  public ResponseEntity<List<Telemetry>> getDeviceTelemetry(@PathVariable Long id) {
    // Kiểm tra thiết bị có tồn tại không
    if (deviceService.findById(id).isEmpty()) {
      return ResponseEntity.notFound().build();
    }
    
    List<Telemetry> telemetryList = telemetryService.getTelemetryByDeviceId(id);
    return ResponseEntity.ok(telemetryList);
  }
}