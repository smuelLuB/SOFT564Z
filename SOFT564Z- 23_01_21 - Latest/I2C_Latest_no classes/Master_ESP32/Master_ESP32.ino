#include <Wire.h>
#include <WiFi.h>"
#include "string.h"

/// Define the Pins used for I2C communication ///
#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11

//////////////////////////////////////////////////////// web page //////////////////////////////////////////////
//Home Network credentials
const char* ssid     = "VM1947422";
const char* password = "kv7skFkdcms4";

// IP address: 192.168.0.32

// Set the web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String httpRequest;

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 2000;        // Define timeout time in milliseconds (2000ms = 2s)

// String Variable for storing the servo data as a string
String servoValueString = String(5);

// Variables to store the current command states
bool ForwardState = false;
bool BackwardState = false;
bool SensorReadState = false;

// Variables to store the current old command states
bool oldForwardState = false;
bool oldBackwardState = false;
bool oldSensorReadState = false;

int redLED = 25; 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
  int motorDir;
  int readSensors;
  int servoVal;
} CommandStruct;

typedef struct {
  byte dist;
  byte temp;
  byte humid;
} DataStruct;

CommandStruct commandCode = {0}; 
DataStruct SensorData;

void setup() {
  Serial.begin(115200);  // start serial for output
  Wire.begin();          //Begin I2C.
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);

  setupWeb();

  // Initialise the command Code values to zero. 
  commandCode.motorDir = 0;
  commandCode.readSensors = 0;
  commandCode.servoVal = 0;

}

void loop() {

  WebPageControls();
  commandCodeHandler();

  if(ForwardState != oldForwardState){
    masterWriter();
  }

  if(BackwardState != oldBackwardState){
    masterWriter();
  }

  if(SensorReadState != oldSensorReadState){
    masterWriter();
  }
  if(SensorReadState == true) {                   // If the sensors are required to be read
    masterReader(3);                            // retrieve the relevant sensor data to update the webpage. 
  }

  oldForwardState = ForwardState;
  oldBackwardState = BackwardState;
  oldSensorReadState = SensorReadState;

}

// handles the writing of data via I2C
void masterWriter() {
  int numBytes = 3;
  byte dOut[numBytes];
  dOut [0] = commandCode.motorDir;
  dOut [1] = commandCode.readSensors;
  dOut [2] = commandCode.servoVal;
  
  Wire.beginTransmission(SLAVE_ADDRESS);
  int numBytesSent = Wire.write(dOut, numBytes);        //send Data to slave, number of bytes sent=numBytes.
  byte result = Wire.endTransmission();                 // if zero returned then tranmission was successful.
  
}

void masterReader(int numBytes) {
  byte dIn[numBytes];
  int bytesRec = Wire.requestFrom(SLAVE_ADDRESS, numBytes, true);                    // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  Wire.readBytes(dIn, bytesRec);
  if (bytesRec != numBytes) {
    Serial.println("Error: Master Didnt reccieve all bytes");
    digitalWrite(redLED, LOW);
  }
  SensorData.dist  = dIn[0];
  SensorData.temp  = dIn[1];
  SensorData.humid = dIn[2];
}

void commandCodeHandler() {

  //commandCode &= 0;                        //reset the code, ready to be updated.

  //check the states so know what to send.
  if (ForwardState == true && BackwardState == false) {
    commandCode.motorDir = 0x2;                                           // Command code for turning motors in forward direction
  }
  else if (BackwardState == true && ForwardState == false) {
    commandCode.motorDir = 0x1;                                          // Command code for turning motors in backward direction
  } else {
    commandCode.motorDir = 0x0;                                           // Command code for motors off.
  }
  if (SensorReadState == true) {
    //request sensor data
    commandCode.readSensors= 0x1;
  } else {
    commandCode.readSensors= 0x0;
  }
}


///////////////////////////////////// WEB PAGE //////////////////////////////////////////////////////////////

void setupWeb() {

  currentTime = millis();
  previousTime = currentTime;
    
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    currentTime = millis();
    if(currentTime - previousTime >= timeoutTime) {
      WiFi.begin(ssid, password);     //try again to  connect 
    }
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //previousTime = 0;              //reset value to zero. 
  server.begin();
}

void WebPageControls() {

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("Client Request.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        httpRequest += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP httpRequests always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (httpRequest.indexOf("GET /Forward/on") >= 0) {
              Serial.println("ForwardState on");
              ForwardState = true; //mutally exclusive
              BackwardState = false;

            } else if (httpRequest.indexOf("GET /Forward/off") >= 0) {
              Serial.println("ForwardState off");
              ForwardState = false;

            } else if (httpRequest.indexOf("GET /Backward/on") >= 0) {
              Serial.println("BackwardState on");
              BackwardState = true; //mutally exclusive                   
              ForwardState = false;
              
            } else if (httpRequest.indexOf("GET /Backward/off") >= 0) {
              Serial.println("BackwardState off");
              BackwardState = false;

            } else if (httpRequest.indexOf("GET /SensorRead/on") >= 0) {
              Serial.println("SensorReadState on");
              SensorReadState = true;
              masterReader(3);                              //read current sensors value ready to be displayed

            } else if (httpRequest.indexOf("GET /SensorRead/off") >= 0) {
              Serial.println("SensorReadState off");
              SensorReadState = false;
            }

            //Modify and Display the contents of the WebPage.
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
           
            if(SensorReadState == true) {
            client.println("<meta http-equiv=\"refresh\" content=\"10\">");
            }
            
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 20px 50px;");        //green
            client.println("text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
           //slider
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 600px;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Display Forward state
            // If the ForwardState is true, it displays the Green button
            if (ForwardState == true) {
              client.println("<p><a href=\"/Forward/off\"><button class=\"button \">FORWARD</button></a></p>");
            } else {
              client.println("<p><a href=\"/Forward/on\"><button class=\"button button2\">FORWARD</button></a></p>");
            }

            // Display Backward state 
            // If the BackwardState is true, it displays the Green button
            if (BackwardState == true) {
              client.println("<p><a href=\"/Backward/off\"><button class=\"button \">BACKWARD</button></a></p>");
            } else {
              client.println("<p><a href=\"/Backward/on\"><button class=\"button button2 \">BACKWARD</button></a></p>");
            }
            //Servo Position and slider 
            client.println("<p> Servo Position: <span id=\"servoPos\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + servoValueString + "\"/>");

            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");
           
            //GET /?value=180& HTTP/1.1192
            if (httpRequest.indexOf("GET /?value=") >= 0) {
              int pos1 = httpRequest.indexOf('=');
              int pos2 = httpRequest.indexOf('&');
              servoValueString = httpRequest.substring(pos1 + 1, pos2);       //Between these two index strings is a servo position value. 
              commandCode.servoVal = servoValueString.toInt();
              masterWriter();           // send servo command for fast response.
            }

            // Display read sensor state            
            // If the SensorReadState is true, it displays the Green button
            if (SensorReadState == true) {
              client.println("<p><a href=\"/SensorRead/off\"><button class=\"button \">READ SENSORS</button></a></p>");
            } else {
              client.println("<p><a href=\"/SensorRead/on\"><button class=\"button button2 \">READ SENSORS</button></a></p>");
            }          
     
            //Display current Sensor Values
            if (SensorReadState == true) {
              client.println("<p> Distance CM: " + String(SensorData.dist) + "</p>");
              client.println("<p> Temperature: " + String(SensorData.temp) + "</p>");
              client.println("<p> Humidity: " + String(SensorData.humid) + "</p>");
            }
            client.println("</body></html>");
            
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the httpRequest variable
    httpRequest = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

/*
  byte MasterReader(int numBytes) {
  int bytesRec =Wire.requestFrom(SLAVE_ADDRESS, numBytes, true);                    // address, number of bytes wish to receive and then a stop condition to release I2C bus.
  byte x;
  while (Wire.available()) {
    x = Wire.read();
  }
  return x;
  }
*/
