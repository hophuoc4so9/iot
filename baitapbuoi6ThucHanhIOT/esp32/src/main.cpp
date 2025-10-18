#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <time.h>
// RGB LED (NeoPixel) support
#include <Adafruit_NeoPixel.h>
// DHT22 support

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// --- WiFi / MQTT settings (adjust to your network/broker) ---
const char* WIFI_SSID = "Pz";
const char* WIFI_PASS = "111222333";

const char* MQTT_HOST = "10.30.233.17";
const int MQTT_PORT = 1883;
const char* MQTT_USER = ""; // optional
const char* MQTT_PASSWD = ""; // optional

// Namespace for topics (set to something meaningful for your broker)
const char* NS = "iot/demo";

// --- Pins / NeoPixel ---
const int RGB_PIN = 48;         // NeoPixel data pin
const int RGB_COUNT = 1;        // number of pixels
Adafruit_NeoPixel strip(RGB_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);

// Current color state
int ledR = 0, ledG = 0, ledB = 0;
bool lightOn = false;

// MQTT topics
String topicDeviceCmd;
String topicDeviceState;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

String getTimestamp() {
  char buf[32];
  snprintf(buf, sizeof(buf), "%lu", millis());
  return String(buf);
}

void publishDeviceState() {
  StaticJsonDocument<200> doc;
  doc["ts"] = getTimestamp();
  doc["light"] = lightOn ? "on" : "off";
  JsonArray rgb = doc.createNestedArray("led_rgb");
  rgb.add(ledR);
  rgb.add(ledG);
  rgb.add(ledB);
  String payload;
  serializeJson(doc, payload);
  
  bool success = mqttClient.publish(topicDeviceState.c_str(), payload.c_str(), true);
  if (success) {
    Serial.print("📤 Published state to ");
    Serial.print(topicDeviceState);
    Serial.print(": ");
    Serial.println(payload);
  } else {
    Serial.println("❌ Failed to publish state");
  }
}

void applyColor(int r, int g, int b) {
  ledR = r; ledG = g; ledB = b;
  // Update lightOn status based on color
  lightOn = (r > 0 || g > 0 || b > 0);
  strip.setPixelColor(0, strip.Color(ledR, ledG, ledB));
  strip.show();
}

void handleDeviceCommand(char* topic, byte* payload, unsigned int length) {
  Serial.print("📥 MQTT Command received on topic: ");
  Serial.println(topic);
  Serial.print("📦 Payload: ");
  Serial.write(payload, length);
  Serial.println();
  
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("❌ JSON parse error: ");
    Serial.println(err.c_str());
    return;
  }

  // Handle "light" command (on/off)
  if (doc.containsKey("light")) {
    String lightCmd = doc["light"].as<String>();
    if (lightCmd == "on") {
      Serial.println("💡 Turning LED ON (White)");
      lightOn = true;
      applyColor(255, 255, 255);
      publishDeviceState();
    } else if (lightCmd == "off") {
      Serial.println("💡 Turning LED OFF");
      lightOn = false;
      applyColor(0, 0, 0);
      publishDeviceState();
    }
  }
  // Handle RGB color command
  else if (doc.containsKey("led_rgb")) {
    JsonArray arr = doc["led_rgb"].as<JsonArray>();
    if (arr.size() >= 3) {
      int r = arr[0];
      int g = arr[1];
      int b = arr[2];
      Serial.printf("🎨 Setting LED color: R=%d, G=%d, B=%d\n", r, g, b);
      applyColor(r, g, b);
      publishDeviceState();
    }
  } 
  // Handle "off" command (backward compatibility)
  else if (doc.containsKey("off")) {
    bool off = doc["off"];
    if (off) {
      Serial.println("💡 Turning LED OFF");
      applyColor(0,0,0);
      publishDeviceState();
    }
  }
}

void connectWiFi() {
  Serial.print("📶 Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("✅ WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
}

void connectMqtt() {
  Serial.printf("🔌 Connecting to MQTT broker: %s:%d\n", MQTT_HOST, MQTT_PORT);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  
  while (!mqttClient.connected()) {
    Serial.print(".");
    if (mqttClient.connect("esp32-rgb-client", MQTT_USER, MQTT_PASSWD)) {
      Serial.println();
      Serial.println("✅ MQTT connected!");
      break;
    }
    delay(200);
  }
  
  mqttClient.subscribe(topicDeviceCmd.c_str());
  Serial.print("📨 Subscribed to topic: ");
  Serial.println(topicDeviceCmd);
  
  publishDeviceState();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.println("   ESP32 IoT MQTT Controller v1.0");
  Serial.println("========================================");
  
  strip.begin();
  strip.setBrightness(80);
  strip.show();
  Serial.println("💡 NeoPixel initialized");

  topicDeviceCmd = String(NS) + "/device/cmd";
  topicDeviceState = String(NS) + "/device/state";
  
  Serial.println("\n📋 MQTT Configuration:");
  Serial.printf("   Namespace: %s\n", NS);
  Serial.printf("   Command Topic: %s\n", topicDeviceCmd.c_str());
  Serial.printf("   State Topic: %s\n", topicDeviceState.c_str());
  Serial.println();

  connectWiFi();
  mqttClient.setCallback(handleDeviceCommand);
  connectMqtt();
  
  Serial.println("\n✨ System ready! Waiting for commands...\n");
}

void loop() {
  if (!mqttClient.connected()) connectMqtt();
  mqttClient.loop();
  delay(10);
}