/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/uart.h"

#define SPP_TAG "SPP_ECHO_DEMO"
#define SPP_SERVER_NAME "ESP32_SPP_SERVER"
#define EXCAMPLE_DEVICE_NAME "ESP32_BT_BRIDGE"

// Định nghĩa các thông số cho UART (kết nối với máy tính qua USB)
#define UART_PORT_NUM      UART_NUM_0
#define UART_BUF_SIZE      (1024)

// Handle của kết nối SPP, dùng để gửi dữ liệu
static uint32_t spp_handle = 0;

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        // Đặt tên cho thiết bị Bluetooth
        esp_bt_dev_set_device_name(EXCAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        // Bắt đầu một server SPP
        esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "Client đã kết nối, handle: %" PRIu32, param->srv_open.handle);
        // Lưu lại handle của kết nối để có thể gửi dữ liệu đi
        spp_handle = param->srv_open.handle;
        break;
    case ESP_SPP_DATA_IND_EVT:
        // Sự kiện này được gọi khi có dữ liệu từ Bluetooth gửi đến
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len=%d handle=%d",
                 param->data_ind.len, param->data_ind.handle);
        // In dữ liệu nhận được ra log (tùy chọn)
        esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
        // Gửi dữ liệu nhận từ Bluetooth ra cổng UART (máy tính)
        uart_write_bytes(UART_PORT_NUM, (const char *)param->data_ind.data, param->data_ind.len);
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "Client đã ngắt kết nối, handle: %" PRIu32, param->close.handle);
        // Reset handle khi kết nối bị đóng
        spp_handle = 0;
        break;
    // Các sự kiện khác không cần xử lý trong ví dụ này
    default:
        break;
    }
}

// Task dùng để đọc dữ liệu từ UART và gửi qua Bluetooth
static void uart_task(void *arg)
{
    uint8_t* data = (uint8_t*) malloc(UART_BUF_SIZE);
    while (1) {
        // Đọc dữ liệu từ UART
        int len = uart_read_bytes(UART_PORT_NUM, data, (UART_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0';
            ESP_LOGI(SPP_TAG, "Nhận từ UART: %s", (char *)data);
            
            // Nếu có kết nối Bluetooth (spp_handle khác 0) thì gửi dữ liệu đi
            if (spp_handle != 0) {
                esp_spp_write(spp_handle, len, data);
            }
        }
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // --- Cấu hình UART ---
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Cài đặt driver UART
    uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // --- Cấu hình Bluetooth ---
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    // Tạo task để xử lý việc đọc dữ liệu từ UART
    xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
    
    ESP_LOGI(SPP_TAG, "Bluetooth Started! Ready to pair...");
}
