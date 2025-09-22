package com.example.iot_backend.repository;

import com.example.iot_backend.model.Telemetry;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.List;
import java.util.Optional;

public interface TelemetryRepository extends JpaRepository<Telemetry, Long> {
    
    @Query("SELECT t FROM Telemetry t WHERE t.device.id = :deviceId ORDER BY t.ts DESC")
    List<Telemetry> findByDeviceIdOrderByTsDesc(@Param("deviceId") Long deviceId);
    
    @Query("SELECT t FROM Telemetry t WHERE t.device.id = :deviceId ORDER BY t.ts DESC LIMIT 1")
    Optional<Telemetry> findLatestByDeviceId(@Param("deviceId") Long deviceId);
}
