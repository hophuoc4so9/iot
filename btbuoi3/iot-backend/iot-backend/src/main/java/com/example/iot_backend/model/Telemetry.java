package com.example.iot_backend.model;

import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "telemetry")
public class Telemetry {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;

  @ManyToOne(optional = false)
  @JoinColumn(name = "device_id")
  private Device device;

  private Instant ts = Instant.now();

  @Column(columnDefinition = "text")
  private String data; // JSON string: {"temp": 26.5, "hum": 70}

  public Long getId() { return id; }
  public void setId(Long id) { this.id = id; }
  public Device getDevice() { return device; }
  public void setDevice(Device device) { this.device = device; }
  public Instant getTs() { return ts; }
  public void setTs(Instant ts) { this.ts = ts; }
  public String getData() { return data; }
  public void setData(String data) { this.data = data; }
}