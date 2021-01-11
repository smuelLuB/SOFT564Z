
// include this library's description file
#include "I2CSlave.h"

int slaveI2C::_dataOut = 0;  // provide definition for static member varaibale.
int slaveI2C::_dataIn = 0;   // provide definition for static member varaibale.

///// ~Constructor
slaveI2C::slaveI2C (int slaveAddress) {
  _slaveAddress = slaveAddress;
  Wire.begin(_slaveAddress);
  Wire.onRequest(slaveWriter);  //attatch the slave writer function when Request interrupt fires.
  Wire.onReceive(slaveReader);  //attatch the slave reader function when Receive interrupt fires.
}
void slaveI2C::update_dataOut(int dataOut) {
  _dataOut = dataOut;
}
void slaveI2C::update_dataIn(int dataIn) {
  _dataIn = dataIn;
}
int slaveI2C::get_dataOut() {
  return _dataOut;
}
int slaveI2C::get_dataIn() {
  return _dataIn;
}
void slaveI2C::slaveWriter()  {
  int dO = slaveI2C::get_dataOut();
  Wire.write(dO);
}
void slaveI2C::slaveReader(int howMany) {
  for (int i = 0; i < howMany; i++) {
    int  dI = Wire.read();                       // read data into temporary variable for data coming In.
    slaveI2C::update_dataIn(dI);
  }
}
