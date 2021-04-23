/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

/* ESP32 & ESP8266 Setup
 *  File-Preferences Addtional Board Manager:
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  ESP32 board is: DOTIT ESP32 DEVKIT V1
 *  
 *  Push "BOOT" button on ESP32 to upload sketch
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "DarkSideOfMoon";
const char *password = "LHS202021";

WiFiServer server(80);

boolean LED_status = false;
String LED_status_str = "OFF";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 500;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
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

void loop() {
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

/* Old HTML
            // the content of the HTTP response follows the header:
            client.print("<a href=\"/H\"><h1>LED ON</h1></a><br>");
            client.print("<a href=\"/L\"><h1>LED OFF</h1></a><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
*/

           // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for LED  
            client.println("<p>LED - State <b>" + LED_status_str + "</b></p>");

            // If the output26State is off, it displays the ON button       
            if (LED_status==true) {
              //client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
              client.println("<p><a href=\"/L\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              //client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
              client.println("<p><a href=\"/H\"><button class=\"button\">ON</button></a></p>");
           } 
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
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
          LED_status = false;
          LED_status_str = "OFF";
        }
        if (currentLine.endsWith("Client")) {
          if (LED_status)
            client.print("LED on\r");
          else
             client.print("LED off\r");
        }
      }// big while statement
    
    } // big if statement
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
