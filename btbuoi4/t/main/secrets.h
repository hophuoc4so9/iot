/*
 * Secrets configuration for IoT Dashboard ESP32 Firmware
 * 
 * IMPORTANT SECURITY NOTICE:
 * This file contains sensitive configuration data including WiFi credentials
 * and MQTT authentication details. 
 * 
 * ⚠️  DO NOT COMMIT THIS FILE TO VERSION CONTROL ⚠️
 * 
 * Instructions:
 * 1. Copy this file and rename it to "secrets.h"
 * 2. Fill in your actual WiFi and MQTT credentials
 * 3. Add "secrets.h" to your .gitignore file
 * 4. Keep this template file for reference
 */

#ifndef SECRETS_H
#define SECRETS_H

// WiFi Network Configuration
// Replace with your actual WiFi network credentials
#define WIFI_SSID "TDMU"
#define WIFI_PASSWORD ""

// MQTT Broker Authentication
// Leave empty strings if your MQTT broker doesn't require authentication
#define MQTT_USERNAME ""               // MQTT username (optional)
#define MQTT_PASSWORD ""               // MQTT password (optional)

// Advanced MQTT Configuration (Optional)
// Uncomment and modify if you need custom MQTT settings
// #define MQTT_BROKER_CUSTOM "192.168.1.100"    // Custom MQTT broker IP
// #define MQTT_PORT_CUSTOM 1883                  // Custom MQTT port
// #define MQTT_CLIENT_ID_PREFIX "IoTDevice_"     // Custom client ID prefix

// OTA (Over-The-Air) Update Configuration (Future Feature)
// Uncomment when OTA updates are implemented
// #define OTA_PASSWORD "YOUR_OTA_PASSWORD"       // Password for OTA updates
// #define OTA_HOSTNAME "esp32-iot-device"        // Hostname for OTA

// Device-Specific Secrets
// Add any device-specific credentials or API keys here
// #define API_KEY "YOUR_API_KEY"                 // External API key (if needed)
// #define DEVICE_SECRET "YOUR_DEVICE_SECRET"     // Device authentication secret

/*
 * Example Configuration:
 * 
 * For home WiFi network:
 * #define WIFI_SSID "MyHomeNetwork"
 * #define WIFI_PASSWORD "MySecurePassword123"
 * 
 * For MQTT broker with authentication:
 * #define MQTT_USERNAME "iot_user"
 * #define MQTT_PASSWORD "iot_password_2024"
 * 
 * For public/guest network (no password):
 * #define WIFI_SSID "PublicWiFi"
 * #define WIFI_PASSWORD ""
 */

// Validation Macros
#ifndef WIFI_SSID
  #error "WIFI_SSID must be defined in secrets.h"
#endif

#ifndef WIFI_PASSWORD
  #error "WIFI_PASSWORD must be defined in secrets.h (can be empty string for open networks)"
#endif

// Security Notes:
// 1. Use strong WiFi passwords (WPA2/WPA3)
// 2. Consider using MQTT over TLS/SSL for production
// 3. Rotate MQTT credentials regularly
// 4. Use network segmentation for IoT devices
// 5. Monitor device connections and unusual activity

#endif // SECRETS_H