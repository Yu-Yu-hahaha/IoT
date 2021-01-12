#include <Servo.h> // include the Servo library
#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

Servo myServo;  // create a servo object

int angle;   // variable to hold the angle for the servo motor

void setup() {
  //start the Modbus RTU server, with (slave) id 1
  if (!ModbusRTUServer.begin(1, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // configure 10 holding registers with starting address 0x00 (40000)
  ModbusRTUServer.configureHoldingRegisters(0x00, 10);

  myServo.attach(9); // attaches the servo on pin 9 to the servo object
  angle = 15; //initialize angle
  myServo.write(angle);
  Serial.begin(9600); // open a serial connection to your computer 
}

void loop() {
  ModbusRTUServer.poll();
  // e.g. ModbusRTUServer holdingRegister and coil read
  int remote_angle = ModbusRTUServer.holdingRegisterRead(0x01);
  //e.g. ModbusRTUServer holdingRegister and coil write
  ModbusRTUServer.holdingRegisterWrite(0x02, angle);
  
  myServo.write(remote_angle);

  // wait for the servo to get there
  delay(1000);
}
