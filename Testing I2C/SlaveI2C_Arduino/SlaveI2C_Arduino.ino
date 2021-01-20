#include <Wire.h>
#include "string.h"

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
DataStruct SensorData;   // Global structure for storing sensor data.

// unsigned short DataIn = 0;
//unsigned short DataOut = 0;

//int DataOut[];   //global buffer for data going out.


void setup() {
  slaveSetup();
  Serial.begin(115200);
  SensorData.dist = 45;
  SensorData.temp = 25;
  SensorData.humid = 90;
}

void loop() {
  Serial.println(commandCode.motorDir);
  Serial.println(commandCode.readSensors);
  Serial.println(commandCode.servoVal);
  delay(1000);
}

void slaveSetup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(slaveWriter);
  Wire.onReceive(slaveReader);
}

void slaveWriter()  {
  byte dOut[2];
  dOut[0] = SensorData.dist;
  dOut[1] = SensorData.temp;
  dOut[2] = SensorData.humid;
  Wire.write(dOut, 3);
}
/*
  void slaveReader() {
  int numBytes = Wire.available();
  byte dIn[numBytes];                   // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  Wire.readBytes(dIn, numBytes);
  commandCode.motorDir    = dIn[0];
  commandCode.readSensors = dIn[1];
  commandCode.servoVal    = dIn[2];
  }

*/

void slaveReader() {

  int idx = 0;
  //int dIn[Wire.available()];
  int dIn[3];

  while (Wire.available()) {
    dIn[idx] = Wire.read();
    idx++;
  }
  // update command codes
  commandCode.motorDir    = dIn[0];
  commandCode.readSensors = dIn[1];
  commandCode.servoVal    = dIn[2];
}



/*
  void slaveReader() {
  int numBytes = Wire.available();
  byte dIn[numBytes];                   // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  Wire.readBytes(dIn, numBytes);

  commandStruct.motorDir    = dIn[0];
  commandStruct.readSensors = dIn[1];
  commandStruct.servoVal    = dIn[2];
  }
*/

/*
  void slaveReader() {

  int numBytes = Wire.available();
  Serial.println(numBytes);
  // unsigned short dIn = 0;
  int c[numBytes];
  for (int i = 0; i <numBytes; i++) {
    c[i] = Wire.read();
    Serial.println(c[i]);
  }

  }
*/
