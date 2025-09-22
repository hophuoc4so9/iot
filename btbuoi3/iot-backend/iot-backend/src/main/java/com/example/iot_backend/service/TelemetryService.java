package com.example.iot_backend.service;

import com.example.iot_backend.model.Device;
import com.example.iot_backend.model.Telemetry;
import com.example.iot_backend.repository.DeviceRepository;
import com.example.iot_backend.repository.TelemetryRepository;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.time.Instant;
import java.util.List;
import java.util.Optional;

@Service
public class TelemetryService {
  private static final Logger log = LoggerFactory.getLogger(TelemetryService.class);
  private final TelemetryRepository telemetryRepo;
  private final DeviceRepository deviceRepo;
  private final ObjectMapper objectMapper;

  public TelemetryService(TelemetryRepository telemetryRepo, DeviceRepository deviceRepo) {
    this.telemetryRepo = telemetryRepo;
    this.deviceRepo = deviceRepo;
    this.objectMapper = new ObjectMapper();
  }

  public void saveTelemetry(Long deviceId, String jsonData) {
    Optional<Device> deviceOpt = deviceRepo.findById(deviceId);
    if (deviceOpt.isPresent()) {
      // Kiểm tra cảnh báo nhiệt độ trước khi lưu
      checkTemperatureAlert(deviceId, jsonData);
      
      Telemetry t = new Telemetry();
      t.setDevice(deviceOpt.get());
      t.setTs(Instant.now());
      t.setData(jsonData);
      telemetryRepo.save(t);
    } else {
      log.warn("Device not found with id={}", deviceId);
    }
  }

  private void checkTemperatureAlert(Long deviceId, String jsonData) {
    try {
      JsonNode jsonNode = objectMapper.readTree(jsonData);
      if (jsonNode.has("temp")) {
        double temperature = jsonNode.get("temp").asDouble();
        if (temperature > 30.0) {
          log.warn("ALERT HOT! Device ID: {}, Temperature: {}°C", deviceId, temperature);
        }
      }
    } catch (Exception e) {
      log.error("Failed to parse temperature from telemetry data: {}", jsonData, e);
    }
  }

  public List<Telemetry> getTelemetryByDeviceId(Long deviceId) {
    return telemetryRepo.findByDeviceIdOrderByTsDesc(deviceId);
  }

  public Optional<Telemetry> getLatestTelemetryByDeviceId(Long deviceId) {
    return telemetryRepo.findLatestByDeviceId(deviceId);
  }
}