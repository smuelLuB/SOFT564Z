
// include this library's description file
#include "I2CMaster.h"

int masterI2C::_dataOut = 0;  // provide definition for static member varaibale.
int masterI2C::_dataIn = 0;   // provide definition for static member varaibale.
int masterI2C::_numBytes = 0;
int masterI2C::_slaveAddress = 0;

///////////////////////////////////////////////////////// Constructor ///////////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances
masterI2C::masterI2C (int SCL, int SDA, int clockFrequency) {
  _SCL = SCL;
  _SDA = SDA;
  _clockFrequency = clockFrequency;
  Wire.begin();
  Wire.setClock(_clockFrequency);
}
////////////////////////////////////////////////////////// private methods /////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////// public methods /////////////////////////////////////////////////////////////////
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
