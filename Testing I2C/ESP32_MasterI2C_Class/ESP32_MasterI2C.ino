#include <Wire.h>
#include "I2CMaster.h"
#include "string.h"

#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11
#define CLOCK_FREQUENCY 100000

///////// Functions //////////
 void I2CHandler();
void extractSensorData(unsigned int data);

///////////////////////////////////////////////////////////

unsigned int I2CcommandCode = 0;
//------------------------------ Command For sending over I2C ----------------------------//

// ¦----------------------------------------------¦
// ¦  Servo Value ¦ Read sensor ¦ Motor Direction ¦    Motor Direction ¦ forward [1 & 0]
// ¦----------------------------------------------¦                    ¦ backward[0 & 1]
// ¦    value     ¦ 0-Off, 1-On ¦   fwd & bkwrd   ¦                    ¦ Off     [0 & 0]
// ¦ ---------------------------------------------¦
//
//------------------------------ ---------------------------------------------------------//

typedef struct {
  byte dist;
  byte temp;
  byte humid;
} DataStruct;

DataStruct SensorData;   // Global structure for storing sensor data. 

// Auxiliar variables to store the current output state
bool ForwardState = false;
bool BackwardState = false;
bool SensorReadState = false;

int servoVal = 0;

const int output25 = 25;

masterI2C master(ESP_SCL, ESP_SDA, CLOCK_FREQUENCY);

void setup() {
  Serial.begin(115200);  // start serial for output
  // Initialize the output variables as outputs
  pinMode(output25, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output25, LOW);
}

int cunt = 0;

void loop() {

  I2CHandler();
  Serial.println(I2CcommandCode, BIN);
  master.sendDataOut(SLAVE_ADDRESS, I2CcommandCode);
  
  /*cunt++;
   if (cunt == 12){
    ForwardState = !ForwardState; 
    SensorReadState = !SensorReadState; 
    cunt =0;

   Serial.println("Hiiiiyyyaaa");
   }

   delay(500);
  */
  /*
  unsigned int dataIn = master.requestDataIn(SLAVE_ADDRESS, 3);      // 2 byte long
  extractSensorData(dataIn);
  //Serial.println(SensorData.dist);
  //Serial.println(SensorData.temp);
  // Serial.println(SensorData.humid);
  */
}

void I2CHandler() {
  
  I2CcommandCode &= 0;                        //reset the code, ready to be updated.
  //check the states so know what to send.
  if (ForwardState == true && BackwardState == false) {
    I2CcommandCode = 0x2;                                           // Command code for turning motors in forward direction
  }
  else if (BackwardState == true && ForwardState == false) {
    I2CcommandCode = 0x1;                                           // Command code for turning motors in backward direction
  } else {
    I2CcommandCode = 0x0;                                           // Command code for motors off.
  }
  if (SensorReadState == true) {
    //request sensor data
    I2CcommandCode += (1 << 2);
  } else {
    I2CcommandCode += (0 << 2);
  }
  
  //I2CcommandCode += (servoVal << 3);                       //Send servo position in MSB
}

//update global struct 
void  extractSensorData(unsigned int data) {
    SensorData.dist = data & 0xFF;     // Lowest byte is distance value 
    SensorData.temp = data & 0xFF00;   // Second byte is temp value 
    SensorData.humid =  data & 0xFF000; // highest byte is the humidity value.  
}
