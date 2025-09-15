#include <HardwareSerial.h>

HardwareSerial SerialPort(2); //Sử dụng dòng lệnh này nếu bạn dùng UART 2


char number  = ' ';
int LED = 15;

void setup()
{
  SerialPort.begin(15200, SERIAL_8N1, 16, 17);
  pinMode(LED, OUTPUT);
}
void loop()
{
  if (SerialPort.available())
  {
    char number = SerialPort.read();
    if (number == '0') {
      digitalWrite(LED, LOW);
    }
    if (number == '1') {
      digitalWrite(LED, HIGH);
    }
  }
}