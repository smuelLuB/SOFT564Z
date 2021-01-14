#include <Wire.h>
#include "I2CMaster.h"
#include "string.h"

#define ESP_SCL 22
#define ESP_SDA 21
#define SLAVE_ADDRESS 11
#define CLOCK_FREQUENCY 100000

////////////////////// web.h //////////////////////////////
// include Wi-Fi library
#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// GPIO the servo is attached to
static const int servoPin = 5;

// Network credentials
const char* ssid     = "VM1947422";
const char* password = "kv7skFkdcms4";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

// Auxiliar variables to store the current output state
String output25State = "off";
String output27State = "off";
String output33State = "off";

// Assign output variables to GPIO pins
const int output25 = 25;
const int output27 = 27;

const int output33 = 33;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


///////// Functions 
void setupWeb();
void WebPageControls(); 

///////////////////////////////////////////////////////////

//------------------------------ Command For sending over I2C ----------------------------//
// 1 - Drive Motors Forward 
// 2 - Drive Motors Backward
// 3 - Turn Left (Left Motor On for 3 seconds)   - not sure
// 4 - Turn Right (Right Motor On for 3 seconds) - not sure
// 5 - Move Servo to "x" position determined by slider
// 6 - Retrieve Sensor Data
//------------------------------ Command For sending over I2C ----------------------------//

int commandCode = 0; 


masterI2C master(ESP_SCL, ESP_SDA, CLOCK_FREQUENCY); 

void setup() {
  Serial.begin(115200);  // start serial for output
  setupWeb(); 
  myservo.attach(servoPin);  // attaches the servo on the servoPin to the servo object
}

void loop() {
  int data = master.requestDataIn(SLAVE_ADDRESS,1);
  Serial.println(data);
  delay(500); // 0.5 second delay between readings 
  WebPageControls(); 

}

void setupWeb() {
  // Initialize the output variables as outputs
  pinMode(output25, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(output33, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output25, LOW);
  digitalWrite(output27, LOW);
  digitalWrite(output33, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void WebPageControls() {
  
    WiFiClient client = server.available();   // Listen for incoming clients
    
    if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /25/on") >= 0) {
              Serial.println("GPIO 25 on");
              output25State = "on";
              digitalWrite(output25, HIGH);
            } else if (header.indexOf("GET /25/off") >= 0) {
              Serial.println("GPIO 25 off");
              output25State = "off";
              digitalWrite(output25, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            } else if (header.indexOf("GET /33/on") >= 0) {
              Serial.println("GPIO 33 on");
              output33State = "on";
              digitalWrite(output33, HIGH);
            } else if (header.indexOf("GET /33/off") >= 0) {
              Serial.println("GPIO 33 off");
              output33State = "off";
              digitalWrite(output33, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;");
            client.println(".button3 {background-color: #555555;}</style></head>");

            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 600px;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
                     
            // Display current state, and ON/OFF buttons for GPIO 25  
            client.println("<p> Left Motor " + output25State + "</p>");
            // If the output27State is on, it displays the ON button       
            if (output25State=="on") {
              client.println("<p><a href=\"/25/off\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/25/on\"><button class=\"button button2 button3\">OFF</button></a></p>");
            }
            client.println("</body></html>");                       //is this neccesary here? 
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p> Right Motor " + output27State + "</p>");
            // If the output27State is on, it displays the ON button       
            if (output27State=="on") {
              client.println("<p><a href=\"/27/off\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/on\"><button class=\"button button2 button3\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for GPIO 25  
            client.println("<p> Third Button " + output33State + "</p>");
            // If the output27State is on, it displays the ON button       
            if (output33State=="on") {
              client.println("<p><a href=\"/33/off\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/33/on\"><button class=\"button button2 button3\">OFF</button></a></p>");
            }

            client.println("</body></html>");

            client.println("<p>Position: <span id=\"servoPos\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+valueString+"\"/>");
            
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");
           
            client.println("</body></html>");  
            
            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              
              //Rotate the servo
              myservo.write(valueString.toInt());
              Serial.println(valueString); 
            }   
            
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
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}  
