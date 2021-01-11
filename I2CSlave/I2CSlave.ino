#include "string.h"
#include "I2CSlave.h"
#include <hcsr04.h>

#define TRIG_PIN 14 //ANALOGUE A0
#define ECHO_PIN 15 //ANALOGUE A1
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 10, 4200);


slaveI2C slave(11);

void setup() {
  Serial.begin(115200);
}

void loop() {
  int DistCm = hcsr04.distanceInMillimeters() / 10; // update the distance in cm
  slave.update_dataOut(DistCm);
  delay(100);
}
