#include "string.h"
#include "I2CSlave.h"
#include <hcsr04.h>
#include <dht.h>
#include <Servo.h>


#define TRIG_PIN 14 //ANALOGUE A0
#define ECHO_PIN 15 //ANALOGUE A1
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 10, 4200);

#define DHT11_PIN 8
dht DHT;                                      //define instance of temp/humid sensor

Servo myservo;  // create servo object to control a servo
// GPIO the servo is attached to
static const int servoPin = 3;

typedef struct {
  byte dist;
  byte temp;
  byte humid;
} DataStruct;

typedef struct {
  unsigned int motorDir;
  unsigned int readSensors;
  unsigned int servoVal;
} commandStruct;

DataStruct SensorData;   // Global structure for storing sensor data.
commandStruct CommandCode;

slaveI2C slave(11);

//fucntions//
unsigned int packageSensorData();
void extractDataIn(unsigned int dataIn);

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);  // attaches the servo on the servoPin to the servo object
}

void loop() {

  unsigned int dIn = slave.get_dataIn();
  Serial.println(dIn, BIN);

  //extractDataIn(dIn); //retrieve command codes and extract data in.
  /*Serial.print("Motor Direction = ");
  Serial.println(CommandCode.motorDir);
  Serial.print("Read Sensors = ");
  Serial.println(CommandCode.readSensors);
  Serial.print("Servo Value = ");
  Serial.println(CommandCode.servoVal);
  */
  //myservo.write(CommandCode.servoVal); //convert string to integer.
/*
  if (CommandCode.motorDir == 0x1) {
    //motors forward
  } else if (CommandCode.motorDir == 0x2) {
    //motors backward
  } else {
    //motors off
  }

  if (CommandCode.readSensors == 1) {
    SensorData.dist = hcsr04.distanceInMillimeters() / 10; // update the distance in cm
    int errCheck = DHT.read11(DHT11_PIN);
    SensorData.temp = DHT.temperature;
    SensorData.humid = DHT.humidity;
    unsigned int dOut = packageSensorData();
    slave.update_dataOut(dOut);               //update the data to be sent to master upon request.
  } else if (CommandCode.readSensors == 0) {
    slave.update_dataOut(0);   //send nul value.
  }

  */
}

unsigned int packageSensorData() {
  unsigned int data = SensorData.dist;
  data += (SensorData.temp << 2);
  data += (SensorData.humid << 4);
  return data;
}

void extractDataIn(unsigned int dataIn) {
  CommandCode.motorDir = dataIn & 0x3;    // Lowest 2bits are distance value
  CommandCode.readSensors = dataIn & 0x4;    // 3rd bit is read sensor command temp value
  CommandCode.servoVal =  dataIn & 0x7F8;  // highest 2bytes are the humidity value.
}
