/*
 * IoT Dashboard - ESP32 Firmware (ESP-IDF)
 * COPILOT_PRIMER.md Implementation - ESP-IDF Code for ESP32-C3/S3
 * 
 * Hardware:
 * - ESP32-C3 or ESP32-S3
 * - DHT22 temperature/humidity sensor
 * - Built-in LED for control demonstration
 * 
 * MQTT Topics:
 * - Publish: iot/classroom/{DEVICE_ID}/telemetry
 * - Subscribe: iot/classroom/{DEVICE_ID}/control
 * - Publish: iot/classroom/{DEVICE_ID}/ack
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"
#include "cJSON.h"
#include "config.h"
#include "secrets.h"

static const char *TAG = "IoT_Dashboard";

// DHT22 simulation (replace with actual DHT22 driver)
typedef struct {
    float temperature;
    float humidity;
} dht_data_t;

// Device state
static bool led_state = false;
static dht_data_t sensor_data = {0.0, 0.0};

// FreeRTOS handles
static EventGroupHandle_t wifi_event_group;
static QueueHandle_t control_queue;

// Event bits
#define WIFI_CONNECTED_BIT BIT0
#define MQTT_CONNECTED_BIT BIT1

// MQTT Topics
static char telemetry_topic[128];
static char control_topic[128];
static char ack_topic[128];
static char status_topic[128];

// Control message structure
typedef struct {
    char command[32];
    bool value;
} control_msg_t;

// Function declarations
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void initialize_hardware(void);
static void initialize_wifi(void);
static void initialize_mqtt(void);
static void setup_mqtt_topics(void);
static void telemetry_task(void *pvParameters);
static void control_task(void *pvParameters);
static void wifi_monitor_task(void *pvParameters);
static bool read_dht22(dht_data_t *data);
static void send_telemetry_data(void);
static void handle_control_message(const char *data);
static void send_ack_response(const char *command, bool success, const char *message);
static void send_device_status(const char *status);

void app_main(void)
{
    ESP_LOGI(TAG, "🚀 IoT Dashboard - ESP32 Firmware Starting...");
    ESP_LOGI(TAG, "📱 Device ID: %s", DEVICE_ID);
    ESP_LOGI(TAG, "🔧 Board: %s", BOARD_TYPE);

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize hardware
    initialize_hardware();

    // Create event group and queue
    wifi_event_group = xEventGroupCreate();
    control_queue = xQueueCreate(10, sizeof(control_msg_t));

    // Initialize WiFi
    initialize_wifi();

    // Initialize MQTT
    initialize_mqtt();

    // Setup MQTT topics
    setup_mqtt_topics();

    // Create FreeRTOS tasks
    xTaskCreate(telemetry_task, "telemetry_task", 4096, NULL, 5, NULL);
    xTaskCreate(control_task, "control_task", 4096, NULL, 5, NULL);
    xTaskCreate(wifi_monitor_task, "wifi_monitor_task", 2048, NULL, 3, NULL);

    ESP_LOGI(TAG, "✅ Initialization complete!");
    ESP_LOGI(TAG, "📊 Starting telemetry transmission...");
}

static void initialize_hardware(void)
{
    ESP_LOGI(TAG, "🔧 Initializing hardware...");

    // Initialize LED
    gpio_config_t led_config = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_config));
    gpio_set_level(LED_PIN, 0);
    led_state = false;
    ESP_LOGI(TAG, "💡 LED initialized on GPIO%d", LED_PIN);

    // Test LED
    ESP_LOGI(TAG, "💡 Testing LED...");
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ESP_LOGI(TAG, "✅ Hardware initialization complete");
}

static void initialize_wifi(void)
{
    ESP_LOGI(TAG, "📶 Initializing WiFi...");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASSWORD);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "🌐 Connecting to WiFi SSID: %s", WIFI_SSID);
}

static void initialize_mqtt(void)
{
    ESP_LOGI(TAG, "📡 Initializing MQTT...");
    ESP_LOGI(TAG, "🌐 Broker: %s:%d", MQTT_BROKER, MQTT_PORT);

    esp_mqtt_client_config_t mqtt_cfg = {};
    char broker_uri[128];
    snprintf(broker_uri, sizeof(broker_uri), "mqtt://%s", MQTT_BROKER);
    mqtt_cfg.broker.address.uri = broker_uri;
    mqtt_cfg.broker.address.port = MQTT_PORT;

    if (strlen(MQTT_USERNAME) > 0) {
        mqtt_cfg.credentials.username = MQTT_USERNAME;
        mqtt_cfg.credentials.authentication.password = MQTT_PASSWORD;
    }

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL));

    ESP_LOGI(TAG, "✅ MQTT client initialized");
}

static void setup_mqtt_topics(void)
{
    snprintf(telemetry_topic, sizeof(telemetry_topic), "iot/classroom/%s/telemetry", DEVICE_ID);
    snprintf(control_topic, sizeof(control_topic), "iot/classroom/%s/control", DEVICE_ID);
    snprintf(ack_topic, sizeof(ack_topic), "iot/classroom/%s/ack", DEVICE_ID);
    snprintf(status_topic, sizeof(status_topic), "iot/classroom/%s/status", DEVICE_ID);

    ESP_LOGI(TAG, "📋 MQTT Topics configured:");
    ESP_LOGI(TAG, "📤 Telemetry: %s", telemetry_topic);
    ESP_LOGI(TAG, "📥 Control: %s", control_topic);
    ESP_LOGI(TAG, "📤 ACK: %s", ack_topic);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "🔄 WiFi disconnected, attempting reconnection...");
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "✅ WiFi connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        
        // Start MQTT client when WiFi is connected
        esp_mqtt_client_start(mqtt_client);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "✅ MQTT connected!");
        xEventGroupSetBits(wifi_event_group, MQTT_CONNECTED_BIT);
        
        // Subscribe to control topic
        esp_mqtt_client_subscribe(mqtt_client, control_topic, 1);
        ESP_LOGI(TAG, "📥 Subscribed to: %s", control_topic);
        
        // Send device online status
        send_device_status("online");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "📡 MQTT disconnected");
        xEventGroupClearBits(wifi_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "📥 MQTT message received on %.*s", event->topic_len, event->topic);
        if (strncmp(event->topic, control_topic, event->topic_len) == 0) {
            handle_control_message(event->data);
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "❌ MQTT error occurred");
        break;

    default:
        break;
    }
}

static void telemetry_task(void *pvParameters)
{
    while (1) {
        // Wait for MQTT connection
        EventBits_t bits = xEventGroupWaitBits(wifi_event_group, MQTT_CONNECTED_BIT, 
                                               pdFALSE, pdTRUE, portMAX_DELAY);
        
        if (bits & MQTT_CONNECTED_BIT) {
            send_telemetry_data();
        }
        
        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_INTERVAL));
    }
}

static void control_task(void *pvParameters)
{
    control_msg_t control_msg;
    
    while (1) {
        if (xQueueReceive(control_queue, &control_msg, portMAX_DELAY)) {
            bool success = false;
            char response_message[128];
            
            ESP_LOGI(TAG, "🎮 Control command: %s (value: %s)", 
                     control_msg.command, control_msg.value ? "true" : "false");
            
            if (strcmp(control_msg.command, "LED_ON") == 0) {
                led_state = true;
                gpio_set_level(LED_PIN, 1);
                success = true;
                strcpy(response_message, "LED turned ON");
            } 
            else if (strcmp(control_msg.command, "LED_OFF") == 0) {
                led_state = false;
                gpio_set_level(LED_PIN, 0);
                success = true;
                strcpy(response_message, "LED turned OFF");
            } 
            else if (strcmp(control_msg.command, "LED_TOGGLE") == 0) {
                led_state = !led_state;
                gpio_set_level(LED_PIN, led_state ? 1 : 0);
                success = true;
                snprintf(response_message, sizeof(response_message), "LED toggled %s", led_state ? "ON" : "OFF");
            } 
            else {
                success = false;
                snprintf(response_message, sizeof(response_message), "Unknown command: %s", control_msg.command);
            }
            
            send_ack_response(control_msg.command, success, response_message);
            
            if (success) {
                ESP_LOGI(TAG, "✅ Command executed: %s", response_message);
                // Send immediate telemetry update
                vTaskDelay(pdMS_TO_TICKS(100));
                send_telemetry_data();
            } else {
                ESP_LOGI(TAG, "❌ Command failed: %s", response_message);
            }
        }
    }
}

static void wifi_monitor_task(void *pvParameters)
{
    while (1) {
        EventBits_t bits = xEventGroupGetBits(wifi_event_group);
        
        if (!(bits & WIFI_CONNECTED_BIT)) {
            ESP_LOGW(TAG, "⚠️  WiFi not connected, monitoring...");
        }
        
        vTaskDelay(pdMS_TO_TICKS(30000)); // Check every 30 seconds
    }
}

static bool read_dht22(dht_data_t *data)
{
    // Simulate DHT22 sensor reading
    // Replace this with actual DHT22 driver implementation
    static float temp_base = 25.0;
    static float hum_base = 60.0;
    
    // Add some random variation
    data->temperature = temp_base + ((float)(esp_random() % 100) / 100.0 - 0.5) * 10.0;
    data->humidity = hum_base + ((float)(esp_random() % 100) / 100.0 - 0.5) * 20.0;
    
    // Ensure valid ranges
    if (data->temperature < -40) data->temperature = -40;
    if (data->temperature > 80) data->temperature = 80;
    if (data->humidity < 0) data->humidity = 0;
    if (data->humidity > 100) data->humidity = 100;
    
    return true;
}

static void send_telemetry_data(void)
{
    if (!read_dht22(&sensor_data)) {
        ESP_LOGE(TAG, "❌ Failed to read sensor data");
        return;
    }
    
    // Create telemetry JSON
    cJSON *telemetry_json = cJSON_CreateObject();
    cJSON *ts = cJSON_CreateNumber(esp_timer_get_time() / 1000);
    cJSON *temperature = cJSON_CreateNumber(round(sensor_data.temperature * 10.0) / 10.0);
    cJSON *humidity = cJSON_CreateNumber(round(sensor_data.humidity * 10.0) / 10.0);
    cJSON *led = cJSON_CreateBool(led_state);
    cJSON *device_id = cJSON_CreateString(DEVICE_ID);
    
    cJSON_AddItemToObject(telemetry_json, "ts", ts);
    cJSON_AddItemToObject(telemetry_json, "temperature", temperature);
    cJSON_AddItemToObject(telemetry_json, "humidity", humidity);
    cJSON_AddItemToObject(telemetry_json, "led", led);
    cJSON_AddItemToObject(telemetry_json, "deviceId", device_id);
    
    char *json_string = cJSON_Print(telemetry_json);
    
    if (esp_mqtt_client_publish(mqtt_client, telemetry_topic, json_string, 0, 1, 1) != -1) {
        ESP_LOGI(TAG, "📤 Telemetry sent: T=%.1f°C, H=%.1f%%, LED=%s", 
                 sensor_data.temperature, sensor_data.humidity, led_state ? "ON" : "OFF");
    } else {
        ESP_LOGE(TAG, "❌ Failed to send telemetry data");
    }
    
    free(json_string);
    cJSON_Delete(telemetry_json);
}

static void handle_control_message(const char *data)
{
    cJSON *control_json = cJSON_Parse(data);
    if (control_json == NULL) {
        ESP_LOGE(TAG, "❌ Failed to parse control message");
        send_ack_response("PARSE_ERROR", false, "Invalid JSON format");
        return;
    }
    
    cJSON *command_item = cJSON_GetObjectItem(control_json, "command");
    cJSON *value_item = cJSON_GetObjectItem(control_json, "value");
    
    if (!cJSON_IsString(command_item)) {
        send_ack_response("PARSE_ERROR", false, "Missing command field");
        cJSON_Delete(control_json);
        return;
    }
    
    control_msg_t control_msg;
    strncpy(control_msg.command, command_item->valuestring, sizeof(control_msg.command) - 1);
    control_msg.command[sizeof(control_msg.command) - 1] = '\0';
    control_msg.value = cJSON_IsTrue(value_item);
    
    xQueueSend(control_queue, &control_msg, 0);
    
    cJSON_Delete(control_json);
}

static void send_ack_response(const char *command, bool success, const char *message)
{
    cJSON *ack_json = cJSON_CreateObject();
    cJSON *cmd = cJSON_CreateString(command);
    cJSON *succ = cJSON_CreateBool(success);
    cJSON *ts = cJSON_CreateNumber(esp_timer_get_time() / 1000);
    cJSON *msg = cJSON_CreateString(message);
    
    cJSON_AddItemToObject(ack_json, "command", cmd);
    cJSON_AddItemToObject(ack_json, "success", succ);
    cJSON_AddItemToObject(ack_json, "ts", ts);
    cJSON_AddItemToObject(ack_json, "message", msg);
    
    char *json_string = cJSON_Print(ack_json);
    
    if (esp_mqtt_client_publish(mqtt_client, ack_topic, json_string, 0, 0, 0) != -1) {
        ESP_LOGI(TAG, "📤 ACK sent: %s - %s", success ? "SUCCESS" : "FAILED", message);
    } else {
        ESP_LOGE(TAG, "❌ Failed to send ACK response");
    }
    
    free(json_string);
    cJSON_Delete(ack_json);
}

static void send_device_status(const char *status)
{
    cJSON *status_json = cJSON_CreateObject();
    cJSON *device_id = cJSON_CreateString(DEVICE_ID);
    cJSON *stat = cJSON_CreateString(status);
    cJSON *ts = cJSON_CreateNumber(esp_timer_get_time() / 1000);
    
    cJSON_AddItemToObject(status_json, "deviceId", device_id);
    cJSON_AddItemToObject(status_json, "status", stat);
    cJSON_AddItemToObject(status_json, "ts", ts);
    
    char *json_string = cJSON_Print(status_json);
    
    esp_mqtt_client_publish(mqtt_client, status_topic, json_string, 0, 1, 0);
    ESP_LOGI(TAG, "📤 Device status sent: %s", status);
    
    free(json_string);
    cJSON_Delete(status_json);
}