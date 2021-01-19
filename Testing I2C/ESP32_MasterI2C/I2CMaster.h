

// ensure this library description is only included once
#ifndef I2CMaster_h
#define I2CMaster_h


// include types & constants of Wiring core API
#include <Wire.h>

// library interface description

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
  static void update_dataOut(unsigned int dataOut);
  static void update_dataIn(unsigned int dataIn);
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
  void sendDataOut(int slaveAddr, unsigned int dataToSend);      //use to 
};

#endif
