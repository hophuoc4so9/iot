package btbuoi6.btbuoi6.model;

import jakarta.persistence.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "telemetry")
public class Telemetry {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(nullable = false)
    private Long deviceId;
    
    @Column(columnDefinition = "TEXT")
    private String payload;
    
    @Column(nullable = false)
    private LocalDateTime timestamp;

    // Constructors
    public Telemetry() {
        this.timestamp = LocalDateTime.now();
    }

    public Telemetry(Long deviceId, String payload) {
        this.deviceId = deviceId;
        this.payload = payload;
        this.timestamp = LocalDateTime.now();
    }

    // Getters and setters
    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public Long getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(Long deviceId) {
        this.deviceId = deviceId;
    }

    public String getPayload() {
        return payload;
    }

    public void setPayload(String payload) {
        this.payload = payload;
    }

    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }
}
