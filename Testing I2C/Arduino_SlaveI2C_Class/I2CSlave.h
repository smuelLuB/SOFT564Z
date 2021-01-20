

// ensure this library description is only included once
#ifndef I2CMaster_h
#define I2CMaster_h


// include types & constants of Wiring core API
#include <Wire.h>
#include "Arduino.h"

// library interface description

class slaveI2C {

  private:
    //slaveI2C(int _slaveAddress);
    int _slaveAddress;
    static unsigned int _dataOut;
    static unsigned int _dataIn;

    // private methods
    static void slaveWriter();
    static void slaveReader();

  public:
    slaveI2C(int slaveAddress);
    static void update_dataOut(unsigned int dataOut);
    static void update_dataIn(unsigned int dataIn);
    static unsigned int get_dataOut();
    static unsigned int get_dataIn();
}; 

#endif
