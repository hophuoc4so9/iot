#include <HardwareSerial.h>

HardwareSerial SerialPort(2); // Sử dụng dòng lệnh này nếu bạn dùng UART 2


void setup()  
{
  SerialPort.begin(15200, SERIAL_8N1, 16, 17); 
} 
void loop()  
{ 
  SerialPort.print(1);
  delay(5000);
  SerialPort.print(0);
  delay(5000);
}