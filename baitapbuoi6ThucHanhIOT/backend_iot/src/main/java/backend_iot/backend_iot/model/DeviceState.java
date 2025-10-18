package backend_iot.backend_iot.model;

import jakarta.persistence.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "device_state")
public class DeviceState {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String deviceId;
    private Integer ledR;
    private Integer ledG;
    private Integer ledB;
    private String rawPayload;
    private LocalDateTime timestamp;

    public DeviceState() {
    }

    public DeviceState(String deviceId, Integer ledR, Integer ledG, Integer ledB, String rawPayload, LocalDateTime timestamp) {
        this.deviceId = deviceId;
        this.ledR = ledR;
        this.ledG = ledG;
        this.ledB = ledB;
        this.rawPayload = rawPayload;
        this.timestamp = timestamp;
    }

    // Getters and Setters
    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public Integer getLedR() {
        return ledR;
    }

    public void setLedR(Integer ledR) {
        this.ledR = ledR;
    }

    public Integer getLedG() {
        return ledG;
    }

    public void setLedG(Integer ledG) {
        this.ledG = ledG;
    }

    public Integer getLedB() {
        return ledB;
    }

    public void setLedB(Integer ledB) {
        this.ledB = ledB;
    }

    public String getRawPayload() {
        return rawPayload;
    }

    public void setRawPayload(String rawPayload) {
        this.rawPayload = rawPayload;
    }

    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }
}
