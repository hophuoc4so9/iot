#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "led_strip.h"

#define LED_GPIO GPIO_NUM_48

TaskHandle_t HelloWorldTaskHandle = NULL;
TaskHandle_t BlinkyTaskHandle = NULL;

void HelloWorld_Task(void * arg) {
  while (1) {
    printf("Task running: Hello World ..\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Blinky_Task(void * arg) {
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
  led_strip_new_rmt_device( & strip_config, & rmt_config, & led_strip);

  int count_second = 0;

  while (1) {
    count_second += 1;
    switch (count_second) {
    case 10:
      vTaskSuspend(HelloWorldTaskHandle);
      printf("HelloWorld task suspended .. \n");
      break;
    case 14:
      vTaskResume(HelloWorldTaskHandle);
      printf("HelloWorld task resumed .. \n");
      break;
    case 20:
      vTaskDelete(HelloWorldTaskHandle);
      printf("HelloWorld task deleted .. \n");
      break;
    default:
      break;
    }
        led_strip_set_pixel(led_strip, 0, 50, 0, 0); // (R, G, B)
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Tắt LED
        ESP_ERROR_CHECK(led_strip_clear(led_strip));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void app_main(void) {
  xTaskCreatePinnedToCore(Blinky_Task, "Blinky", 4096, NULL, 10, & BlinkyTaskHandle, 0);
  xTaskCreatePinnedToCore(HelloWorld_Task, "HelloWorld", 4096, NULL, 10, & HelloWorldTaskHandle, 1);
}