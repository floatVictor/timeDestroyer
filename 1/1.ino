#include <ESP8266WiFi.h>

int ledPin = 16;      // LED connected to digital pin 9
int ledPin2 = 4;      // LED connected to digital pin 9
int ledPin3 = 5;      // LED connected to digital pin 9


int val = 0;         // variable to store the read value

void setup() {
  pinMode(ledPin, OUTPUT);  // sets the pin as output
  pinMode(ledPin2, OUTPUT);  // sets the pin as output
  pinMode(ledPin3, OUTPUT);  // sets the pin as output

}

void loop() {
  val++;
  if(val>255) val=0;// read the inpt pin
  analogWrite(ledPin, (val+255/3)%255 ); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(ledPin2, (val+255*2/3)%255  ); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(ledPin3, val ); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255

}
