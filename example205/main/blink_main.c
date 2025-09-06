#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

// Chân GPIO kết nối với LED WS2812
#define LED_GPIO GPIO_NUM_48

// Handle cho task
TaskHandle_t BlinkyTaskHandle = NULL;


// Task nhấp nháy LED
void Blinky_Task(void *arg)
{
    // Cấu hình LED strip
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1,
    };

    // Cấu hình RMT cho LED WS2812
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz
    };

    // Tạo handle cho LED strip
    led_strip_handle_t led_strip;
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

    // Xóa LED về trạng thái tắt ban đầu
    led_strip_clear(led_strip);


    while (1)
    {
        // Bật LED màu đỏ
        printf("LED ON\n");
        led_strip_set_pixel(led_strip, 0, 50, 0, 0); // (R, G, B)
        led_strip_refresh(led_strip);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Tắt LED
        printf("LED OFF\n");
        led_strip_clear(led_strip);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Hàm main
void app_main(void)
{
    xTaskCreatePinnedToCore(Blinky_Task, "Blinky", 4096, NULL, 10, &BlinkyTaskHandle, 0);
}
