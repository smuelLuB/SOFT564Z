#include <Wire.h>
#include "I2CMaster.h"
#include "string.h"

#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11
#define CLOCK_FREQUENCY 100000

masterI2C master(ESP_SCL, ESP_SDA, CLOCK_FREQUENCY); 

void setup() {
  Serial.begin(115200);  // start serial for output
}

void loop() {
  int data = master.requestDataIn(SLAVE_ADDRESS,1);
  Serial.println(data);
  delay(500); // 0.5 second delay between readings 

}
