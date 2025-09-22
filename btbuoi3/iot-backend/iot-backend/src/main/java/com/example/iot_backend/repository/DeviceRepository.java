package com.example.iot_backend.repository;

import com.example.iot_backend.model.Device;
import org.springframework.data.jpa.repository.JpaRepository;

public interface DeviceRepository extends JpaRepository<Device, Long> {
}