#include <Wire.h>
#include "string.h"
#include <hcsr04.h>

#define TRIG_PIN 14 //ANALOGUE A0
#define ECHO_PIN 15 //ANALOGUE A1
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 10, 4200);

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

slaveI2C slave(11);

void setup() {
  Serial.begin(115200);
}

void loop() {
  int DistCm = hcsr04.distanceInMillimeters() / 10; // update the distance in cm
  slave.update_dataOut(DistCm);
  delay(100);
}
