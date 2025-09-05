#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

TaskHandle_t HelloWorldTaskHandle = NULL;

void HelloWorld_Task(void *arg)
{
    while (1)
    {
        printf("Task running: Hello World ..\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xTaskCreate(HelloWorld_Task, "HelloWorld", 4096, NULL, 10, &HelloWorldTaskHandle);
    // xTaskCreatePinnedToCore(HelloWorld_Task, "HelloWorld", 4096, NULL, 10, &HelloWorldTaskHandle, 0, NULL);
}