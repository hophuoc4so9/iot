package backend_iot.backend_iot.dto;

import backend_iot.backend_iot.model.DeviceState;

import java.time.LocalDateTime;

public class DeviceStateResponse {
    private Long id;
    private String deviceId;
    private int[] ledRgb;
    private LocalDateTime timestamp;

    public DeviceStateResponse() {
    }

    public DeviceStateResponse(DeviceState state) {
        this.id = state.getId();
        this.deviceId = state.getDeviceId();
        this.ledRgb = new int[]{state.getLedR(), state.getLedG(), state.getLedB()};
        this.timestamp = state.getTimestamp();
    }

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

    public int[] getLedRgb() {
        return ledRgb;
    }

    public void setLedRgb(int[] ledRgb) {
        this.ledRgb = ledRgb;
    }

    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }
}
