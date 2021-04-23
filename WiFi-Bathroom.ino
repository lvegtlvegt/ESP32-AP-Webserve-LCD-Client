#define trigPinR 23       //Right trig pin
#define echoPinR 5        //Right echo pin
#define trigPinL 16       //Left trig pin 
#define echoPinL 17       //Left echo pin
#define sonarDistance 500 //maximum distance that the sensors will take in, KEEP AT 500
#define sonarNum 2        //how many ultrasonic sensors are there, KEEP AT 2
#define ledRed 19         //pin number for red led
#define ledGreen 18       //pin number for green led
#define minDist 0         //Preset minimum distance set for the sensor, KEEP AT ZERO
#define maxDist 125       //maximum distance set for the sensor, measured in centimeters
int distanceR, distanceL;

//int lcdColumns = 16;
//int lcdRows = 2;

#include <NewPing.h>
//#include <SoftwareSerial.h>
//#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//********* LMV WiFi Integration start *************

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

String appName = "Bathroom Counter";

// Set these to your desired credentials.
const char *ssid = "DarkSideOfMoon";
const char *password = "LHS202021";

WiFiServer server(80);

// used for testing WiFi connectivity
boolean LED_status = false;
String LED_status_str = "OFF"; 

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 10000;

//inits the display
//LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//********* LMV WiFi Integration end *************


NewPing sonar[sonarNum] = {
  NewPing(trigPinL, echoPinL, sonarDistance),
  NewPing(trigPinR, echoPinR, sonarDistance)
};

int personCount = 0;
int current_state = 0;

//function process state 4
int state4(int cs){
  digitalWrite(ledGreen, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(ledRed, LOW);   // turn the LED on (HIGH is the voltage level)
  personCount --;
  return 0;
}

//function process state 3
int state3(int cs){
  digitalWrite(ledGreen, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(ledRed, LOW);   // turn the LED on (HIGH is the voltage level)
  personCount ++;
  return 0;
}

//function process state 2
int state2(int cs){
  int next_state = current_state;
  int distanceR = sonar[0].ping_cm();
  if (distanceR < maxDist and distanceR != minDist){
    digitalWrite(ledGreen, HIGH);   // turn the LED on (HIGH is the voltage level)
    next_state = 4;
  }
  return next_state;
}

//function process state 1
int state1(int cs){
  int next_state = current_state;
  int distanceL = sonar[1].ping_cm();
  if (distanceL < maxDist and distanceL != minDist){
    digitalWrite(ledRed, HIGH);   // turn the LED on (HIGH is the voltage level)
    next_state = 3;
  }
  return next_state;
}

//function process state 0
int state0(int cs){
  int next_state = current_state;
  int distanceR = sonar[0].ping_cm();
  if (distanceR < maxDist and distanceR != minDist){
    digitalWrite(ledGreen, HIGH);   // turn the LED on (HIGH is the voltage level)
    next_state = 1;
  }
  else{
      int distanceL = sonar[1].ping_cm();
   if (distanceL < maxDist and distanceL != minDist){
        digitalWrite(ledRed, HIGH);   // turn the LED on (HIGH is the voltage level)
        next_state = 2;
      }
  }
  return next_state;
}


int finite_state(int cs) {
  int next_state = cs;

  switch (cs) {
    case 0:
      next_state = state0(cs); //home state
      break;
 
    case 1:
      next_state = state1(cs); //in state detected
      break;
 
    case 2:
      next_state = state2(cs);//in state confirmed
      break;
 
    case 3:
      next_state = state3(cs);//out state detected
      break;

    case 4:
      next_state = state4(cs);//out state confirmed
      break;
  }
 
  return next_state;
 
}

//********* LMV WiFi Integration start *************
// Initialize the WiFi Access Point
void startupWiFiAP(){
  
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void processWiFiMessage(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()&& currentTime - previousTime <= timeoutTime) {            // loop while the client's connected
     currentTime = millis();
     if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();  
           // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");            // Feel free to change the background-color and font-size attributes to fit your preferences
            
            // Web Page Heading
//            client.println("<body><h1>" + appName + "</h1>");
            client.println("<body><h1>Bathroom Counter</h1>");
            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for LED  
            client.println("<p>LED - State <b>" + LED_status_str + "</b></p>");
//            client.println("<br><p>Person Count: " + personCount + "</p>");
//            client.println("<p>Person Count: " + personCount);
//            client.println("</p>");

            // If the output26State is off, it displays the ON button       
            if (LED_status==true) {
              //client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
              client.println("<p><a href=\"/L\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              //client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
              client.println("<p><a href=\"/H\"><button class=\"button\">ON</button></a></p>");
           } 

            // The HTTP response ends with another blank line
            client.println();
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }


        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }


        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
          LED_status = true;
          LED_status_str = "ON";
          personCount = 0;
          //Serial.println("LED on");
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
          LED_status = false;
          LED_status_str = "OFF";
          personCount = 0;
          //Serial.println("LED off");
        }
        if (currentLine.endsWith("Client")) {
          if (LED_status)
            client.print("LED on " + String(personCount) + "\r");
          else
             client.print("LED off " + String(personCount) + "\r");
        }
      }// big inner if statement
    
    } // big while statement
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  } // big iff statment
} //function
//********* LMV WiFi Integration end *************

void setup() {
  //Wire.begin(SDA,SCL);
  //Wire.begin(10,9);

  //lcd.init();  //Initializes LCD                 
  //lcd.backlight();
  
  Serial.begin(115200);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

//********* LMV WiFi Integration start *************
  startupWiFiAP();
//********* LMV WiFi Integration end *************
}

void loop() {
  //Serial.print("CS: ");
  //Serial.println(current_state);  
  current_state = finite_state(current_state);
  //Serial.println(personCount);
  delay(250);
  //Serial.print("Distance: ");
  //Serial.print(sonar[0].ping_cm());
  //Serial.print(", ");
  //Serial.println(sonar[1].ping_cm());
  //LCD text init
  if ((personCount < 0) or (personCount == 0)){
    personCount = 0;
  }


//********* LMV WiFi Integration start *************
/*
  lcd.setCursor(0,0);
  lcd.print("Person Count: ");
  lcd.print(personCount);
  if (personCount > 3){
    lcd.setCursor(0,1);
    lcd.print("MAXIMUM CAPACITY");}
  if (personCount < 3){
    lcd.setCursor(0,1);
    lcd.print("                ");}
*/ 

  processWiFiMessage();

//********* LMV WiFi Integration end ************* 
}
