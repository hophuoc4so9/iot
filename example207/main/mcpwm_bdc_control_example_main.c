#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "driver/ledc.h"

#include "driver/gpio.h"

#define PWM_GPIO 5 //PWM chung điều khiển tốc độ
#define RPWM_GPIO 6 //Điều khiển chiều quay thuận
#define LPWM_GPIO 7 //Điều khiển chiều quay nghịch
#define LEDC_FREQ 1000 // Tần số PWM 1 kHz
#define LEDC_RES LEDC_TIMER_10_BIT // Độ phân giải 10 bit
void pwm_init() {
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_RES,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = LEDC_FREQ,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config( & ledc_timer);
  ledc_channel_config_t ledc_channel = {
    .gpio_num = PWM_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0
  };
  ledc_channel_config( & ledc_channel);
  gpio_set_direction(RPWM_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(LPWM_GPIO, GPIO_MODE_OUTPUT);
}
void motor_forward(uint32_t duty) {
  gpio_set_level(RPWM_GPIO, 1);
  gpio_set_level(LPWM_GPIO, 0);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
void motor_backward(uint32_t duty) {
  gpio_set_level(RPWM_GPIO, 0);
  gpio_set_level(LPWM_GPIO, 1);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
void motor_stop() {
  gpio_set_level(RPWM_GPIO, 0);
  gpio_set_level(LPWM_GPIO, 0);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
void app_main() {
  pwm_init();
  while (1) {
    printf("Motor forward\n");
    motor_forward(512); // 50% tốc độ
    vTaskDelay(pdMS_TO_TICKS(3000));
    printf("Motor stop\n");
    motor_stop();
    vTaskDelay(pdMS_TO_TICKS(2000));
    printf("Motor backward\n");
    motor_backward(512); // 50% tốc độ
    vTaskDelay(pdMS_TO_TICKS(3000));
    printf("Motor stop\n");
    motor_stop();
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}