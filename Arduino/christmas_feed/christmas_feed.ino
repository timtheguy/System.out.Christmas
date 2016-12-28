/*
 * The Arduino source for System.out.Christmas();
 * December 28, 2016 by timtheguy
 * 
 * Makes a request to a server and pasrses the JSON response from the server to print
 * any new messages posted to the server. A status LED flashes twice if there is no
 * new message. The status LED flashes once for a longer period of time when a new
 * message is found. The message is then printed over a software serial port from the 
 * NodeMCU ESP8266-based module.
 * 
 * 
 */

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_Thermal.h>
#include <SoftwareSerial.h>

/**
 * Define your connection variables here!
 */
const char* ssid     = "XXXXX";
const char* password = "XXXXX";

/**
 * This is where your request server is hosted, without leading "http://" or "www"
 * 
 * Example: mydomain.me
 */
const char* host = "XXXXX"; 

/**
 * This is the specific file that returns the JSON data is located.
 * 
 * Example: /christmas/message.php
 */
String url = "XXXXX";

#define TX_PIN 5 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 4 // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

const int wantedLength = 32;    // The length of our print lines (see print function)

/**
 * Custom function that is used to split lines of text in printable chunks,
 * breaking up lines into a specified length based on the position of spaces.
 */
const char * split (const char * s, const int length)
  {
  // if it will fit return whole thing
  if (strlen (s) <= length)
    return s + strlen (s);

  // searching backwards, find the last space
  for (const char * space = &s [length]; space != s; space--)
    if (*space == ' ')
      return space;
    
  // not found? return a whole line
  return &s [length];        
} // end of split

/**
 * Custom function that takes a string input and prints it to the attached
 * thermal printer. 
 */
void print(const char str[]){
  const char * p = str;

  // keep going until we run out of text
  while (*p){
    // find the position of the space
    const char * endOfLine = split (p, wantedLength);  
  
    // display up to that
    while (p != endOfLine)
      printer.print (*p++);
    
    // finish that line
    printer.println ();
  
    // if we hit a space, move on past it
    if (*p == ' ')
      p++;
  }
} 

/*
 * The function that will initialize our serial port communication and any other I/O
 */
void setup() {
  pinMode(13, OUTPUT);     // Indicator LED used to indicate successful network request
  pinMode(12, OUTPUT);     // Indicator LED used to indicate message received
  
  Serial.begin(115200); // Initialize NodeMCU serial for debugging
  
  mySerial.begin(19200);  // Initialize SoftwareSerial
  printer.begin();       // Init printer (same regardless of serial type)
  
  printer.setTimes(20, 20); // Specify the print times (affects speed and power usage)
  
  delay(100);
 
  // Connect to WiFI network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * The loop() function that will make the request every 5 seconds to the server. 
 * 
 * Includes code modified from http://blog.nyl.io/esp8266-led-arduino/
 */
void loop() {
  delay(5000);
 
  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Ccnnection failed");
    return;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  delay(500);
  
  // Light LED indicator to show request has been made
  digitalWrite(13, HIGH);                
  delay(500);                 
  digitalWrite(13, LOW);  
     
  // Read all the lines of the reply from server and print them to Serial
  String section="header";
  while(client.available()){
  
    String line = client.readStringUntil('\r');
    // Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
      if (line=="\n") { // skips the empty space at the beginning 
        section="section2";
      }
    }else if(section =="section2"){ // skip empty section of data
        section="json"; 
    }else if (section=="json") {  // print the JSON section
      section="ignore";
      String result = line.substring(1);
      if(result== "[]"){ //no new message
        digitalWrite(12, HIGH); 
        Serial.println("Nothing new!");

        // Flash LED twice to indicate no new message
        delay(250);
        digitalWrite(12, LOW);
        delay(250);
        digitalWrite(12, HIGH);
        delay(250);
        digitalWrite(12, LOW);
      }else{
        // Flash LED once to indicate message received
        digitalWrite(12, HIGH);
        delay(1000);
        digitalWrite(12, LOW);

        // Remove opening and closing '[' and ']' for parsing JSON
        result.remove(0, 1);
        result.remove(result.length(), 1);
        
        // Parse JSON
        int size = result.length() + 1;
        char json[size];
        result.toCharArray(json, size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json_parsed = jsonBuffer.parseObject(json);
        if (!json_parsed.success())
        {
          Serial.println("parseObject() failed"); // Make sure you are receiving the proper JSON format
          return;
        }

        Serial.println(json_parsed["added"].asString()); // Output the timestamp to the console
        Serial.println(json_parsed["message"].asString()); // Output the message to the console
        Serial.println(json_parsed["author"].asString()); // Output the author to the console

        printer.justify('C'); // Center our print header
        print("New message:");
        print(json_parsed["added"].asString());
        printer.justify('L'); // Left align our message
        print(json_parsed["message"].asString());
        printer.justify('R'); // Right align our author
        print(json_parsed["author"].asString());

        // Move paper up and out of printer for a better view
        printer.feed(1);
        printer.feed(1);
        printer.feed(1); 
      }
    }
  }
  
  Serial.println();
  Serial.println("Closing connection");
}

