#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define ledPIN 2 

// Khai báo UUID cho Service và Characteristic
// Bạn có thể dùng UUID này hoặc tạo UUID mới
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Lớp Callback để xử lý sự kiện kết nối/ngắt kết nối từ Client
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client Connected!");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client Disconnected!");
    pServer->getAdvertising()->start();
  }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.print("Received Value: ");
        Serial.println(rxValue.c_str());

        if (rxValue == "1") {
          Serial.println("Turning LED ON");
          digitalWrite(ledPIN, HIGH);
        }
        else if (rxValue == "0") {
          Serial.println("Turning LED OFF");
          digitalWrite(ledPIN, LOW);
        }
      }
    }
};

void setup() {
  pinMode(ledPIN, OUTPUT);
  digitalWrite(ledPIN, LOW); 
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // 1. Khởi tạo thiết bị BLE và đặt tên
  BLEDevice::init("ESP32_LED_Control");

  // 2. Tạo BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Tạo BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. Tạo BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE // Chỉ cho phép client ghi dữ liệu
                    );

  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop() {
  delay(1000);
}