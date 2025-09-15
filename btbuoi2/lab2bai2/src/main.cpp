#include <WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_Sensor.h>

#include <DHT.h>

#define DHTPIN 4     // Chân dữ liệu của DHT22 nối với GPIO4 (có thể thay đổi)
#define DHTTYPE DHT22   // Định nghĩa loại cảm biến

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "TDMU";   
const char* password = ""; 
WiFiClient  client;

unsigned long myChannelNumber = 3;
const char * myWriteAPIKey = "YSYJMIVJ4BBAF8FM";

// Biến thời gian
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;


float temperatureC;
float humidity;

void setup() {
  Serial.begin(115200);  //Initialize serial
  dht.begin();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

  // Đọc nhiệt độ và độ ẩm từ DHT22
  temperatureC = dht.readTemperature();
  humidity = dht.readHumidity();
  Serial.print("Temperature (ºC): ");
  Serial.println(temperatureC);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);
  if (isnan(temperatureC) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
    
    

  // Gửi dữ liệu lên ThingSpeak: field 1 = nhiệt độ, field 2 = độ ẩm
  ThingSpeak.setField(1, temperatureC);
  ThingSpeak.setField(2, humidity);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}