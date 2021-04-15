/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>
#include <LiquidCrystal_I2C.h>

WiFiMulti WiFiMulti;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Set these to your desired credentials.
const char *ssid = "DarkSideOfMoon";
const char *password = "LHS202021";

void setup()
{
    Serial.begin(115200);
    delay(10);

    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    

    // We start by connecting to a WiFi network
    WiFiMulti.addAP(ssid, password);

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop()
{
    const uint16_t port = 80;
    const char * host = "192.168.4.1"; // ip or dns
//    const uint16_t port = 1337;
//    const char * host = "192.168.1.10"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);
    //lcd.setCursor(0,0);
    //lcd.print("Connecting...");
    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }
//    lcd.setCursor(0,0);
//    lcd.print("Connected");

    // This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    client.print("Client");
    //uncomment this line to send a basic document request to the server
    //client.print("GET /index.html HTTP/1.1\n\n");

  int maxloops = 0;

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    Serial.println(line);
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print(line);
  }
  else
  {
    Serial.println("client.available() timed out ");
  }

    Serial.println("Closing connection.");
    client.stop();

    Serial.println("Waiting 1 seconds before restarting...");
    delay(1000);
}
