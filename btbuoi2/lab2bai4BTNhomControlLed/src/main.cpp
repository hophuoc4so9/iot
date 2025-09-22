#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 2   // GPIO cho LED

// UUID cho Service và Characteristic
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

// Callback xử lý khi Client kết nối / ngắt kết nối
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client Connected!");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client Disconnected!");
    BLEDevice::startAdvertising(); // Quảng bá lại để client khác kết nối
  }
};

// Callback xử lý khi Client ghi dữ liệu vào Characteristic
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("Received from BLE: ");
      Serial.println(rxValue.c_str());

      // Nếu client gửi "1" thì bật LED, "0" thì tắt LED
      if (rxValue == "1") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      } else if (rxValue == "0") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Starting BLE work...");

  // 1. Khởi tạo BLE
  BLEDevice::init("ESP32S3_BLE_UART_LED");

  // 2. Tạo BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Tạo BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // 4. Tạo BLE Characteristic (Read, Write, Notify)
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  // Gán callback khi có Client ghi dữ liệu
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Thêm Descriptor để client có thể bật Notify
  pCharacteristic->addDescriptor(new BLE2902());

  // 5. Start Service
  pService->start();

  // 6. Quảng bá (Advertising)
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop() {
  
  if (Serial.available()) {
    String rxData = Serial.readStringUntil('\n');
    rxData.trim();
    rxData.replace("\n", ""); 
    rxData.replace("\r", ""); 

    if (deviceConnected) {
      pCharacteristic->setValue(rxData.c_str());
      pCharacteristic->notify();
      Serial.print("Send to BLE Client: ");
      Serial.println(rxData);
    }
  }

  delay(200);
}
