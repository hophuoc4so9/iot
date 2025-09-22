package com.example.iot_backend.model;

import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "devices")
public class Device {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;

  @Column(nullable=false, length=100)
  private String name;

  @Column(length=50)
  private String type;   // sensor, actuator, gateway...

  @Column(length=20)
  private String status; // ONLINE/OFFLINE/UNKNOWN
// 2) Thêm field location (VARCHAR 100) cho bảng devices và seed lại dữ liệu.
  @Column(length=100)
  private String location; // Location of the device

  private Instant createdAt = Instant.now();

  public Long getId() { return id; }
  public void setId(Long id) { this.id = id; }
  public String getName() { return name; }
  public void setName(String name) { this.name = name; }
  public String getType() { return type; }
  public void setType(String type) { this.type = type; }
  public String getStatus() { return status; }
  public void setStatus(String status) { this.status = status; }
  public String getLocation() { return location; }
  public void setLocation(String location) { this.location = location; }
  public Instant getCreatedAt() { return createdAt; }
  public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
}