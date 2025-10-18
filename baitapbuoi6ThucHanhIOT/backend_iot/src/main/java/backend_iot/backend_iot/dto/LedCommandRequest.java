package backend_iot.backend_iot.dto;

public class LedCommandRequest {
    private int[] led_rgb;
    private Boolean off;

    public LedCommandRequest() {
    }

    public LedCommandRequest(int[] led_rgb) {
        this.led_rgb = led_rgb;
    }

    public int[] getLed_rgb() {
        return led_rgb;
    }

    public void setLed_rgb(int[] led_rgb) {
        this.led_rgb = led_rgb;
    }

    public Boolean getOff() {
        return off;
    }

    public void setOff(Boolean off) {
        this.off = off;
    }
}
