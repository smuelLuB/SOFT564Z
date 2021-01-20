#include <Wire.h>
#include "string.h"

#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11

typedef struct {
  int motorDir;
  int readSensors;
  int servoVal;
} CommandStruct;

typedef struct {
  byte dist;
  byte temp;
  byte humid;
} DataStruct;

CommandStruct commandCode;
DataStruct SensorData;

void setup() {
  Serial.begin(115200);  // start serial for output
  Wire.begin();          //Begin I2C.

  commandCode.motorDir = 0x2;
  commandCode.readSensors = 0x1;
  commandCode.servoVal = 180;

}

void loop() {


  masterWriter();
  delay(1000);
  masterReader(3);
  delay(1000); // 1 second delay between readings
  Serial.println(SensorData.dist);
  Serial.println(SensorData.temp);
  Serial.println(SensorData.humid);
  delay(1000);
}

// handles the writing of data via I2C
void masterWriter() {
  int numBytes = 3;
  byte dOut[numBytes];
  dOut [0] = commandCode.motorDir;
  dOut [1] = commandCode.readSensors;
  dOut [2] = commandCode.servoVal;

  Wire.beginTransmission(SLAVE_ADDRESS);
  int numBytesSent = Wire.write(dOut, numBytes);        //send Data to slave, number of bytes sent=numBytes.
  byte result = Wire.endTransmission();                 // if zero returned then tranmission was successful.
  Serial.println(numBytesSent);
  Serial.println(result);
}

void masterReader(int numBytes) {
  byte dIn[numBytes];
  int bytesRec = Wire.requestFrom(SLAVE_ADDRESS, numBytes, true);                    // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  Wire.readBytes(dIn, numBytes);
  if (bytesRec != numBytes) {
    Serial.println("Error: Master Didnt reccieve all bytes");
  }
  SensorData.dist  = dIn[0];
  SensorData.temp  = dIn[1];
  SensorData.humid = dIn[2];
}


/*
  byte MasterReader(int numBytes) {
  int bytesRec =Wire.requestFrom(SLAVE_ADDRESS, numBytes, true);                    // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  byte x;
  while (Wire.available()) {
    x = Wire.read();
  }
  return x;
  }
*/
