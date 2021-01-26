#include <Wire.h>                                 // library used for I2C between microcontrollers. 
#include "string.h"                               // Used for printing and manipulating strings. 
#include <hcsr04.h>                               // Ultrasonic library used for retrieving the ultrasonic distance data. 
#include <dht.h>                                  // The DHT sensor library, used for reading the temperature and humidity values of the sensor. 
#include <Servo.h>                                // Servo library used for moving the servo. 
#include <L298NX2.h>                              // The Motor driver library configured to allow for two motors to be controlled. 

#define SLAVE_ADDRESS 11                          //Slave address of the Arduino Slave device. Master uses this address to send and request data.  

/// sensors and servo ///
#define DHT11_PIN 8                               // Define digital input pin used for recieving DHT sensor data. 
#define TRIG_PIN 14                               // Define Trigger pin used for ultrasonic sensor. Generates the ultrasonic sound pulses. 40,000hz. 
#define ECHO_PIN 15                               // Define Echo pin used for ultrasonic sensor. Echo pin outputs the time in microseconds the sound wave traveled.
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 10, 4200);      // create an ultrasonic object, attatch the correct pins, specifiy the minium and maximum range in mm. 
Servo servo;                                      // create servo object to control a servo
dht DHT;

// Pin definition For motor A. 
const unsigned int IN1_A = 4;
const unsigned int IN2_A = 5;
// Pin definition For motor B. 
const unsigned int IN1_B = 6;
const unsigned int IN2_B = 7;

// Initialise both motors
L298NX2 motors(IN1_A, IN2_A, IN1_B, IN2_B);


//*** Structure for storing the command codes recieved from ESP32. ***//
typedef struct {
  int motorDir;
  int readSensors;
  int servoVal;
} CommandStruct;

//*** Structure for storing the Sesnor data read by arduino upon request from ESP32. ***//
typedef struct {
  byte dist;
  byte temp;
  byte humid;
} DataStruct;

CommandStruct commandCode;        // Global structure for storing current command codes.
DataStruct SensorData;            // Global structure for storing current sensor data.

void setup() {
  
  slaveSetup();                  //Configure the Arduino to operate as an I2C slave device. 
  Serial.begin(115200);          // begin serial terminal communication with baudrate of 115200. 
  servo.attach(3);               // attaches the servo on pin 3 to the servo object to allow for control. 


  // Intialise global structures values to 0. 
  SensorData.dist = 0;
  SensorData.temp = 0;
  SensorData.humid = 0;
  commandCode.motorDir = 0;
  commandCode.readSensors = 0;
  commandCode.servoVal = 0;
}

void loop() {

  servo.write(commandCode.servoVal);       // Move servo
  Serial.println(commandCode.motorDir);    // Print motorDir
  Serial.println(commandCode.readSensors); // Print motorDir
  Serial.println(commandCode.servoVal);    // Print motorDir

  if (commandCode.motorDir == 0x2) {                  // Motor forward command code.
    motors.forward(); //motors forward
  } else if (commandCode.motorDir == 0x1) {           // Motor backward command code.
    motors.backward(); //motors backward
  } else {                                            // Anything else then stop motors
    motors.stop();
  }

  if (commandCode.readSensors == 0x1) {                           // user has requested to read the sensor data. command code recieved.
    //read data
    SensorData.dist = hcsr04.distanceInMillimeters() / 10;        // update the distance in cm
    int checkSuccess = -1;
    while(checkSuccess != 0) {
      checkSuccess = DHT.read11(DHT11_PIN);                       // returns values <0 if read failed, so loop until good value recived. Read the temp and humidty sensor. 
    }
    SensorData.temp = DHT.temperature;                            // update global structure with current value. 
    SensorData.humid = DHT.humidity;

    // print the data, used for testing.
    Serial.print("Temperature = ");                                
    Serial.println(SensorData.temp);
    Serial.print("Humidity = ");
    Serial.println(SensorData.humid);
  }

}

//********** Setup the Arduino as a Slave device for I2C communication. ************// 
void slaveSetup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(slaveWriter);
  Wire.onReceive(slaveReader);
}

//********** Function for writing sensor data out. Form Arduino Slave to ESP Master upon masters request. ************// 
void slaveWriter()  {
  byte dOut[3];                     // create buffer to store the sensor data, of fixed sized. 
  dOut[0] = SensorData.dist;        // store the current ultrasonic distance value. 
  dOut[1] = SensorData.temp;        // store the current DHT temperature value.
  dOut[2] = SensorData.humid;       // store the current DHT humidity value.
  Wire.write(dOut, 3);              // Write the Data buffer out, across the I2C line. 
}

//********** Function for reading command data in. Form ESP Master to Arduino Slave, attatched to Recieve event handler of the Wire.h I2C library. ************// 
void slaveReader() {
  int idx = 0;
  int dIn[Wire.available()];              // Buffer for data coming in, size determined by the number of bytes sent. 
  while (Wire.available()) {              // Whilst theres data to be read. 
    dIn[idx] = Wire.read();               // Read data a byte at a time into data buffer. index determined by idx count variable. 
    idx++;                                // increment counter to store next byte of data in buffer array. 
  }
    // update global command code, so arduino knows the current state.   
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
