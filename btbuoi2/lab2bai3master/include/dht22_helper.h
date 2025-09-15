#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4     // Chân dữ liệu của DHT22 nối với GPIO4 (có thể thay đổi)
#define DHTTYPE DHT22   // Định nghĩa loại cảm biến

DHT dht(DHTPIN, DHTTYPE);

// Hàm duy nhất để khởi tạo và đọc DHT22
bool readDHT22(float &temperature, float &humidity, bool init = false) {
  if (init) {
    dht.begin();
    return true;
  }
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  // Kiểm tra lỗi đọc cảm biến
  if (isnan(temperature) || isnan(humidity)) {
    return false;
  }
  return true;
}
