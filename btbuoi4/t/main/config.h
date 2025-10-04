/*
 * Configuration file for IoT Dashboard ESP32 Firmware (ESP-IDF)
 * 
 * This file contains device-specific configuration and timing settings.
 * Modify these values according to your hardware setup and requirements.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "sdkconfig.h"

// Device Configuration
#define DEVICE_ID "ESP32_001P2N12345"          // Unique device identifier
#define BOARD_TYPE "ESP32-S3"          // Board type for identification

// Hardware Pin Definitions
#ifdef CONFIG_IDF_TARGET_ESP32C3
  #define LED_PIN GPIO_NUM_8             // ESP32-C3 built-in LED (GPIO8)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define LED_PIN GPIO_NUM_48            // ESP32-S3 built-in LED (GPIO48)
#else
  #define LED_PIN GPIO_NUM_2             // Default LED pin
#endif

#define DHT_PIN GPIO_NUM_2               // GPIO pin for DHT22 sensor data line

// Timing Configuration (milliseconds)
#define TELEMETRY_INTERVAL 2000          // Send telemetry every 2 seconds
#define WIFI_RECONNECT_INTERVAL 30000    // Check WiFi connection every 30 seconds
#define MQTT_RECONNECT_INTERVAL 5000     // Try MQTT reconnection every 5 seconds
#define SENSOR_READ_TIMEOUT 2000         // DHT22 sensor read timeout

// MQTT Configuration
#define MQTT_BROKER "localhost"          // MQTT broker hostname/IP
#define MQTT_PORT 1883                   // MQTT broker port (1883 for TCP, 8883 for SSL)
#define MQTT_QOS 1                       // Quality of Service level (0, 1, or 2)
#define MQTT_RETAIN true                 // Retain messages on broker

// Device Behavior Settings
#define MAX_WIFI_RETRY_ATTEMPTS 30       // Maximum WiFi connection attempts
#define MAX_MQTT_RETRY_ATTEMPTS 10       // Maximum MQTT reconnection attempts before restart
#define WATCHDOG_TIMEOUT 60000           // Watchdog timeout (60 seconds)

// Task Configuration
#define TELEMETRY_TASK_STACK_SIZE 4096   // Stack size for telemetry task
#define CONTROL_TASK_STACK_SIZE 4096     // Stack size for control task
#define WIFI_MONITOR_TASK_STACK_SIZE 2048 // Stack size for WiFi monitor task

// Queue Configuration
#define CONTROL_QUEUE_SIZE 10            // Control message queue size

// WiFi Configuration Validation
#if !defined(WIFI_SSID)
  #error "WIFI_SSID must be defined in secrets.h"
#endif

#if !defined(WIFI_PASSWORD)
  #error "WIFI_PASSWORD must be defined in secrets.h"
#endif

// Device ID Validation
#if !defined(DEVICE_ID)
  #error "DEVICE_ID cannot be empty"
#endif

// Board Type Detection
#ifdef CONFIG_IDF_TARGET_ESP32C3
  #ifndef ESP32C3
    #define ESP32C3
  #endif
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #ifndef ESP32S3
    #define ESP32S3
  #endif
#else
  #warning "Unknown ESP32 variant, using default pin configuration"
#endif

// MQTT Topic Templates
#define TELEMETRY_TOPIC_TEMPLATE "iot/classroom/%s/telemetry"
#define CONTROL_TOPIC_TEMPLATE "iot/classroom/%s/control"
#define ACK_TOPIC_TEMPLATE "iot/classroom/%s/ack"
#define STATUS_TOPIC_TEMPLATE "iot/classroom/%s/status"

// JSON Buffer Sizes
#define TELEMETRY_JSON_SIZE 200        // Buffer size for telemetry JSON
#define CONTROL_JSON_SIZE 200          // Buffer size for control JSON
#define ACK_JSON_SIZE 300              // Buffer size for ACK JSON
#define STATUS_JSON_SIZE 200           // Buffer size for status JSON

// Network Timeouts
#define WIFI_CONNECT_TIMEOUT 30000     // WiFi connection timeout (30 seconds)
#define MQTT_CONNECT_TIMEOUT 10000     // MQTT connection timeout (10 seconds)
#define NETWORK_RETRY_DELAY 1000       // Delay between network retry attempts

// Memory and Performance
#define STACK_SIZE 8192                // Task stack size
#define JSON_STATIC_BUFFER true        // Use static JSON buffers for better performance

// Development and Debugging
#ifdef DEBUG_OUTPUT
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(format, ...)
#endif

// Feature Flags
#define ENABLE_WATCHDOG true           // Enable watchdog timer
#define ENABLE_OTA_UPDATES false       // Enable Over-The-Air updates (future feature)
#define ENABLE_DEEP_SLEEP false        // Enable deep sleep mode (battery optimization)
#define ENABLE_LED_INDICATOR true      // Enable LED status indication

// Error Handling
#define RESTART_ON_CRITICAL_ERROR true // Restart ESP32 on critical errors
#define MAX_CONSECUTIVE_ERRORS 5       // Maximum consecutive errors before restart

#endif // CONFIG_H