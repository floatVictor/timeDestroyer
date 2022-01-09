#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>                 // for receiving OSC messages
const char* ssid = "Livebox-9BD0";
const char* pass = "DMsZynzUGq3WwYuJRc";
// Button Input + LED Output
const int ledPin = 4; 
const int ledPin2 = 5; 
const int ledPin3 = 16;                 // D5 pin at NodeMCU
const int boardLed = LED_BUILTIN;      // Builtin LED
WiFiUDP Udp;                           // A UDP instance to let us send and receive packets over UDP
const unsigned int localPort = 4210;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)
const unsigned int destPort = 9000;    // remote port of the target device where the NodeMCU sends OSC to
unsigned int ledState = 1;             // LOW means led is *on*
void setup() {
    Serial.begin(115200);
    // Specify a static IP address for NodeMCU
     // If you erase this line, your ESP8266 will get a dynamic IP address
    WiFi.config(IPAddress(192,168,1,23),IPAddress(192,168,0,1), IPAddress(255,255,255,0)); 
   // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
   Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
   Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
   // LED Output
    pinMode(ledPin, OUTPUT);
    digitalWrite(boardLed, ledState);   // turn *off* led
}
void loop() {
  OSCMessage msgIN;
  int size;
  if((size = Udp.parsePacket())>0 && (int)Udp.read() > 0){
  printf("received udp data : %d \n", (int)Udp.read());
    if(ledState){
      analogWrite(ledPin, 255);
      analogWrite(ledPin2, 255);
      analogWrite(ledPin3, 255);
      msgIN.fill(Udp.read());
      ledState = 0;
    }
    else {
      analogWrite(ledPin, 0);
      analogWrite(ledPin2, 0);
      analogWrite(ledPin3, 0);
      ledState = 1;
    }
  }
  delay(100);
}
