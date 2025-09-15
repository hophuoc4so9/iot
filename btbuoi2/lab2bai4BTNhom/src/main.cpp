#include <Arduino.h>
#include <BLEDevice.h>

#include <BLEServer.h>

#include <BLEUtils.h>

#include <BLE2902.h>

// Khai báo UUID cho Service và Characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer * pServer = NULL;
BLECharacteristic * pCharacteristic = NULL;
bool deviceConnected = false;

// Lớp callback để xử lý các sự kiện kết nối/ngắt kết nối
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer * pServer) {
    deviceConnected = true;
    Serial.println("Connected!");
  };

  void onDisconnect(BLEServer * pServer) {
    deviceConnected = false;
    Serial.println("Disconnected!");
    // Khởi động lại quảng bá để có thể kết nối lại
    BLEDevice::startAdvertising();
  };
};

void setup() {
  Serial.begin(115200);
  Serial.println("Start...");

  // Khởi tạo thiết bị BLE
  BLEDevice::init("ESP32S3_BLE_UART"); // Đặt tên cho thiết bị của bạn

  // Tạo BLE Server
  pServer = BLEDevice::createServer();
  pServer -> setCallbacks(new MyServerCallbacks());

  // Tạo BLE Service
  BLEService * pService = pServer -> createService(SERVICE_UUID);

  // Tạo BLE Characteristic
  pCharacteristic = pService -> createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  // Thêm Descripter để client có thể nhận được thông báo (notify)
  pCharacteristic -> addDescriptor(new BLE2902());

  // Bắt đầu Service
  pService -> start();

  // Bắt đầu quảng bá (advertising) để thiết bị khác có thể tìm thấy
  BLEAdvertising * pAdvertising = BLEDevice::getAdvertising();
  pAdvertising -> addServiceUUID(SERVICE_UUID);
  pAdvertising -> setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop() {
  if (Serial.available()) {
    String rxData = Serial.readStringUntil('\n');
    if (deviceConnected) {
      rxData.trim();
      pCharacteristic->setValue(rxData.c_str());
      pCharacteristic->notify(); // Gửi dữ liệu qua BLE
      Serial.print("Send to: ");
      Serial.println(rxData);
    }
  }

  // Xử lý khi có dữ liệu từ BLE Client gửi đến
  if (pCharacteristic -> getValue().length() > 0) {
    std::string rxValue = pCharacteristic -> getValue();
    Serial.print("Received from BLE: ");
    Serial.println(rxValue.c_str());
    // Xóa giá trị để tránh xử lý lặp lại
    pCharacteristic -> setValue("");
  }

  delay(500); // Tăng delay để giảm tải cho CPU
}