#include <WiFi.h>

// LED optional
const int led = 21;

// Holds door state
bool state = false;
String doorState;

unsigned long previousMillis = 0; 
const long interval = 20000; // 20 giây

const char* ssid = "HO TUONG VSIP";
const char* password = "111222333";
const char* host = "maker.ifttt.com";
const char* apiKey = "lkCHpWBG3NPAKJ-GJ20uPNvfujGWMIrsmZy0wBDHqhm";

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    
    state = !state;
    if (state) {
      doorState = "closed";
    } else {
      doorState = "open";
    }
    digitalWrite(led, !state);

    Serial.println(state);
    Serial.println(doorState);

    // Gửi email qua IFTTT
    Serial.print("connecting to ");
    Serial.println(host);
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    String url = "/trigger/door_status/with/key/";
    url += apiKey;

    Serial.print("Requesting URL: ");
    Serial.println(url);
    String body = "value1=" + doorState;

    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Content-Type: application/x-www-form-urlencoded\r\n" + 
                 "Content-Length: " + body.length() + "\r\n\r\n" +
                 body + "\r\n");
  }
}
