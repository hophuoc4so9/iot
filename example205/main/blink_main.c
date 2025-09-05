#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_strip.h"

// Chân GPIO kết nối với LED WS2812
#define LED_GPIO GPIO_NUM_48

TaskHandle_t BlinkyTaskHandle = NULL;

// Tag dùng cho log
static const char *TAG = "LED_BLINK";
void Blinky_Task(void *arg)
{
    ESP_LOGI(TAG, "Initializing LED strip");

    // Cấu hình LED strip
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1,
    };

    // Cấu hình RMT (Remote Control) cho LED WS2812
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz
    };

    // Tạo handle cho LED strip
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "LED strip initialized");

    // Xóa LED về trạng thái tắt ban đầu
    ESP_ERROR_CHECK(led_strip_clear(led_strip));

    while (1)
    {
        // Bật LED màu đỏ
        ESP_LOGI(TAG, "Turning LED RED");
        led_strip_set_pixel(led_strip, 0, 50, 0, 0); // (R, G, B)
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Tắt LED
        ESP_LOGI(TAG, "Turning LED OFF");
        ESP_ERROR_CHECK(led_strip_clear(led_strip));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void app_main(void)
{
    xTaskCreatePinnedToCore(Blinky_Task, "Blinky", 4096, NULL, 10, &BlinkyTaskHandle, 0);
}
