/*
   Send and receive OSC messages between NodeMCU and another OSC speaking device.
   Send Case: Press a physical button (connected to NodeMCU) and get informed about it on your smartphone screen
   Receive Case: Switch an LED (connected to NodeMCU) on or off via Smartphone
   
   Written by Jackson Campbell <jcampbell@calarts.edu>
   Modified by Donovan Keith <dkeith@calarts.edu>
   
   for [Augmenting Realities](https://sites.google.com/view/augmentingrealities/home)
   
   Based on sketch by Fabian Fiess in November 2016
   Inspired by Oscuino Library Examples, Make Magazine 12/2015
*/

// Imports
// ==============================================================================

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// [OSC for Arduino Library](https://github.com/CNMAT/OSC)
#include <OSCMessage.h>                // for sending OSC messages
#include <OSCBundle.h>                 // for receiving OSC messages

       
#define sensorPin 0
unsigned long myTimer;
unsigned long myOldRdv;

// Settings
// ==============================================================================


// WiFi Network Settings
// ---------------------

char wifiNetworkName[] = "iPhone de Victor";
char wifiPassword[] = "ouiouioui";

byte mac[6];
// OSC Server/Client Addresses and Ports
// --------------------------------------

const IPAddress destIp(172,20,10,6); // remote IP of the target device
const unsigned int destPort = 9000;    // remote port of the target device where the NodeMCU sends OSC to

const IPAddress localIp(172,20,10,3);
const unsigned int localPort = 8666;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)

const IPAddress networkGateway(192,168,0,1);
const IPAddress subnet(255, 255, 255, 0);


// Board Pin Mapping
// ---------------------

// Button Input + LED Output    
const int testPin = 3;
const int redPin = 16;             
const int greenPin = 5;
const int bluePin = 4;  

int pinCount = 3;
int pins[] = {redPin, greenPin, bluePin};
int state;

const int boardLed = LED_BUILTIN;      // Builtin LED


// Globals
// ==============================================================================

// State Variables
// ---------------------

boolean buttonChanged = false;
int buttonVal = 1;
unsigned int ledState = 1;             // LOW means led is *on*



// Objects
// ---------------------

WiFiUDP Udp;                           // A UDP instance to let us send and receive packets over UDP


// Program Flow
// ==============================================================================

void setup() {
  
  Serial.begin(115200);
  WiFi.setAutoConnect(false);
  setupPins();
  ledDemo();
  connectToWifi();
  myOldRdv = millis();
  }

void loop() {
  
  myTimer = millis() - myOldRdv;
  if(myTimer > 20){
    detectSendOSC();
    myOldRdv = millis();
  }
  receiveOSC();
}

// Helpers
// ==============================================================================

void connectToWifi() {
  // Specify a static IP address for NodeMCU - only needeed for receiving messages)
  // If you erase this line, your ESP8266 will get a dynamic IP address
  // the parameters of Wifi.config are ( STATIC_IP, DNS_IP (router's IP), DOMAIN_IP)
  WiFi.config(localIp, networkGateway, subnet);
  delay(100);
  if(!WiFi.config(localIp, networkGateway, subnet)){
    Serial.println("STA Failed to configure");
  }

  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifiNetworkName);
  WiFi.begin(wifiNetworkName, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("cc je suis un nodeMcu :)");
  Serial.print("adresse IP : ");
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  Serial.print("adresse MAC : ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
}

void setupPins() {
  // buttonInput + LED Output
  pinMode(sensorPin, INPUT);
  pinMode(testPin, OUTPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(boardLed, OUTPUT);
}

void blinkRGB(int period){
  // Blinks RGB LED Red, Green, then Blue  
  for (int i=0; i<pinCount; i++){
    digitalWrite(pins[i], HIGH);
    delay(period);
    digitalWrite(pins[i], LOW);
  }
  delay(period);
}

void blinkWhite(int period){
  // Flash White
  for (int i=0; i<pinCount; i++){
    digitalWrite(pins[i], HIGH);
  }

  delay(period);

  // Turn off all LEDs
  for (int i=0; i<pinCount; i++){
    digitalWrite(pins[i], LOW);
  }
}

void ledDemo() {
  // Blinks the RGB LED to establish that all is working.
  int period = 500;  // Milliseconds

  Serial.println("Testing RGB LED...");

  blinkRGB(period);
  blinkRGB(period);
  blinkRGB(period);
  
  blinkWhite(period);

  Serial.println("Testing complete.");

}
// OSC I/O
// ==============================================================================

void receiveOSC() {
  char my_buffer[100];
  OSCBundle msgIN;
  int size;
  if ((size = Udp.parsePacket()) > 0) {
    //Serial.println("receiving something");    
    while (size--)
      msgIN.fill(Udp.read());

    if (!msgIN.hasError()) {
      msgIN.dispatch("/r", Rled); //rgb led pwm color mixing functions
      msgIN.dispatch("/g", Gled); //rgb led pwm color mixing functions
      msgIN.dispatch("/b", Bled); //rgb led pwm color mixing functions
    }
  }
}


// Message Handlers
// ---------------------

void Rled(OSCMessage &msg) {
  int value = msg.getInt(0); //TouchOSC only sends floats, but store as int
  //Serial.println(value);
  analogWrite(redPin, value); //Set the led level by writing pwm level to pin
}

void Gled(OSCMessage &msg) {
  int value = msg.getInt(0); //TouchOSC only sends floats, but store as int
  analogWrite(greenPin, value); //Set the led level by writing pwm level to pin
}

void Bled(OSCMessage &msg) {
  int value = msg.getInt(0); //TouchOSC only sends floats, but store as int
  analogWrite(bluePin, value); //Set the led level by writing pwm level to pin
}

// detector fuction

void detectSendOSC(){
  state = analogRead(sensorPin);
  //Serial.println(state);
  if(state == 1){
    analogWrite(testPin,255);
  }
  else{ 
    analogWrite(testPin,0);
  }
  
  OSCMessage msgOut("/2/detect/");
  msgOut.add(state); // add the value to the OSC routing
  Udp.beginPacket(destIp, destPort);//Format UDP message
  msgOut.send(Udp);
  Udp.endPacket();
  msgOut.empty();
}
