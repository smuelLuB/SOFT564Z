#include <Wire.h>
#include "string.h"
#include <hcsr04.h>
#include <Servo.h>
#include <dht.h>
#include <L298NX2.h>

#define SLAVE_ADDRESS 11

/// sensors and servo ///
#define DHT11_PIN 8
#define TRIG_PIN 14 //ANALOGUE A0
#define ECHO_PIN 15 //ANALOGUE A1
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 10, 4200);
Servo servo;  // create servo object to control a servo
dht DHT;

// Pin definition
const unsigned int IN1_A = 4;
const unsigned int IN2_A = 5;

const unsigned int IN1_B = 6;
const unsigned int IN2_B = 7;

// Initialize both motors
L298NX2 motors(IN1_A, IN2_A, IN1_B, IN2_B);

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

void setup() {
  slaveSetup();
  Serial.begin(115200);
  servo.attach(3);  // attaches the servo on pin 9 to the servo object

  SensorData.dist = 0;
  SensorData.temp = 0;
  SensorData.humid = 0;

  commandCode.motorDir = 0;
  commandCode.readSensors = 0;
  commandCode.servoVal = 0;

}

void loop() {

  servo.write(commandCode.servoVal);      // move servo
  Serial.println(commandCode.motorDir);
  Serial.println(commandCode.readSensors);
  Serial.println(commandCode.servoVal);

  if (commandCode.motorDir == 0x2) {
    motors.forward(); //motors forward
  } else if (commandCode.motorDir == 0x1) {
    motors.backward(); //motors backward
  } else {
    motors.stop();
  }

  if (commandCode.readSensors == 0x1) {
    //read data
    SensorData.dist = hcsr04.distanceInMillimeters() / 10; // update the distance in cm
    int check = DHT.read11(DHT11_PIN);
    SensorData.temp = DHT.temperature;
    SensorData.humid = DHT.humidity;

    Serial.print("Temperature = ");
    Serial.println(SensorData.temp);
    Serial.print("Humidity = ");
    Serial.println(SensorData.humid);
  }

}

void slaveSetup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(slaveWriter);
  Wire.onReceive(slaveReader);
}

void slaveWriter()  {
  byte dOut[3];
  dOut[0] = SensorData.dist;
  dOut[1] = SensorData.temp;
  dOut[2] = SensorData.humid;
  Wire.write(dOut, 3);
}

void slaveReader() {
  int idx = 0;
  int dIn[Wire.available()];
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
  commandCode.motorDir    = dIn[0];
  commandCode.readSensors = dIn[1];
  commandCode.servoVal    = dIn[2];
  }
*/
