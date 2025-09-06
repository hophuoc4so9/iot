#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_strip.h"

// Định nghĩa chân GPIO kết nối với LED WS2812
#define LED_GPIO GPIO_NUM_48

void app_main(void)
{
	led_strip_config_t strip_config = {
		.strip_gpio_num = LED_GPIO,
		.max_leds = 1,
	};

	led_strip_rmt_config_t rmt_config = {
		.resolution_hz = 10 * 1000 * 1000,
	};

	// Tạo handle cho dải LED
	led_strip_handle_t led_strip;
	led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

	// Xóa đèn LED về trạng thái tắt trước khi bắt đầu
	led_strip_clear(led_strip);
	while (1)
	{
		// Bật LED màu đỏ
		printf("Turning ON RED\n");
		// Hàm set_pixel vẫn theo thứ tự (R, G, B)
		led_strip_set_pixel(led_strip, 0, 50, 0, 0);
		led_strip_refresh(led_strip);
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		// Tắt LED (bằng cách xóa)
		printf("Turning LED OFF\n");
		led_strip_clear(led_strip);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}