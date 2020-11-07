#include <Wire.h>
#include "string.h"

#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11
#define CLOCK_FREQUENCY 100000

class masterI2C {

  private:
  //private class variables. 
  int _SCL;
  int _SDA;
  int _clockFrequency;
  static int _dataOut;
  static int _dataIn;
  static int _slaveAddress; // changes subject to devices but can only have one value.  
  static int _numBytes;     // number of bytes of data to read from slave. 

  //Private class methods. 
  static void update_dataOut(int dataOut);
  static void update_dataIn(int dataIn);
  static void update_numBytes(int numBytes);
  static void update_slaveAddress(int slaveAddress);
  static int get_dataOut(); 
  static int get_dataIn(); 
  static int get_numBytes();       // used to update the current number of bytes of data the masster is able to request. 
  static int get_slaveAddress();   // used to retrieve current class slave Address. 
  static void masterReader();                   // Private function to perform the reading of incoming data.  
  static void masterWriter();                   // Private function to perform the writing of outgoing data. 
  
  public:
  masterI2C(int SCL, int SDA, int clockFrequency);
  int requestDataIn(int slaveAddr, int numBytes);      //use to request to read incoming data. 
  void sendDataOut(int slaveAddr, int dataToSend);      //use to 
};

int masterI2C::_dataOut = 0;  // provide definition for static member varaibale.
int masterI2C::_dataIn = 0;   // provide definition for static member varaibale.
int masterI2C::_numBytes = 0;
int masterI2C::_slaveAddress = 0;

masterI2C::masterI2C (int SCL, int SDA, int clockFrequency) {
  _SCL = SCL;
  _SDA = SDA;
  _clockFrequency = clockFrequency;
  Wire.begin();
  Wire.setClock(_clockFrequency);
}
void masterI2C::update_dataOut(int dataOut) {
  _dataOut = dataOut;
}
void masterI2C::update_dataIn(int dataIn) {
  _dataIn = dataIn;
}
void masterI2C::update_numBytes(int numBytes) {
  _numBytes = numBytes; 
}
void masterI2C::update_slaveAddress(int slaveAddress) {
  _slaveAddress = slaveAddress;
}
int masterI2C::get_dataOut() {
 return _dataOut; 
}
int masterI2C::get_dataIn() {
 return _dataIn; 
}
int masterI2C::get_slaveAddress() {
  return _slaveAddress;                       //return the address of the current slave address.
}
int masterI2C::get_numBytes() {
  return _numBytes; 
}

// handles the reading of data via I2C
void masterI2C::masterReader() {
  int slaveAddr = masterI2C::get_slaveAddress();      //get the address of the current slave that wish to send data to.
  int numByt =  masterI2C::get_numBytes();
  Wire.requestFrom(slaveAddr, numByt);
  while (Wire.available()) {
    int  dI = Wire.read();                       // read data into temporary variable for data coming In.
    masterI2C::update_dataIn(dI); 
  }
}

// handles the writing of data via I2C
void masterI2C::masterWriter() {
  int dO = masterI2C::get_dataOut();        //retrieve the data to be sent out. 
  int slaveAddr = masterI2C::get_slaveAddress(); 
  Wire.beginTransmission(slaveAddr);
  Wire.write(dO);                           //send Data to slave. 
  Wire.endTransmission();
}

// public function to read data from a slave device. 
int masterI2C::requestDataIn(int slaveAddr, int numBytes) {
  masterI2C::update_slaveAddress(slaveAddr);
  masterI2C::update_numBytes(numBytes); 
  masterI2C::masterReader();          // read from device
  int dIn = masterI2C::get_dataIn();  // retrieve newly read data. 
  return dIn;   
}
//public function to send data to a slave device.
void masterI2C::sendDataOut(int slaveAddr, int dataToSend) {
  masterI2C::update_slaveAddress(slaveAddr);
  masterI2C::update_dataOut(dataToSend);
  masterI2C::masterWriter(); 
}

masterI2C master(ESP_SCL, ESP_SDA, CLOCK_FREQUENCY); 

void setup() {
  Serial.begin(115200);  // start serial for output
}

void loop() {
  int data = master.requestDataIn(SLAVE_ADDRESS,1);
  Serial.println(data);
  delay(500); // 1 second delay between readings 

}
