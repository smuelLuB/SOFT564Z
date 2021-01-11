

// ensure this library description is only included once
#ifndef I2CMaster_h
#define I2CMaster_h


// include types & constants of Wiring core API
#include <Wire.h>

// library interface description

class slaveI2C {

  private:
    //slaveI2C(int _slaveAddress);
    int _slaveAddress;
    static int _dataOut;
    static int _dataIn;

    // private methods
    static void slaveWriter();
    static void slaveReader(int howMany);
    static int get_dataOut();
    static int get_dataIn();

  public:
    slaveI2C(int slaveAddress);
    static void update_dataOut(int dataOut);
    static void update_dataIn(int dataIn);
}; 

#endif
