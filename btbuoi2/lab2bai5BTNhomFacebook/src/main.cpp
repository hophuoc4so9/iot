#include <WiFi.h>    
#include <HTTPClient.h>
#include <UrlEncode.h>

const char* ssid = "HO TUONG VSIP";
const char* password = "111222333";

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "+84349146401";
String apiKey = "025gaHiFjigsJH7z";

void sendMessage(String message){

  // Data to send with HTTP POST
    String url = "https://api.callmebot.com/facebook/send.php?apikey=" + apiKey + "&text=" + urlEncode(message);

  HTTPClient http;
  http.begin(url); 

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.println("Message sent successfully");
  } else {
    Serial.print("Error sending the message. HTTP response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println("Payload: " + payload);
  }

  // Giải phóng tài nguyên
  http.end();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  delay(10000);
  // Send Message to WhatsAPP
  sendMessage("Hello from ESP32!");
   
}

void loop() {
  
}