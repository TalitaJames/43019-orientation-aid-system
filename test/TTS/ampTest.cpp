#include <Arduino.h>

const int ampEnablePin = 4;   

void setup() {
  Serial.begin(115200);            
  pinMode(ampEnablePin, OUTPUT);   // Amp control pin
  digitalWrite(ampEnablePin, HIGH); // Start with amp ON
  Serial.println("ESP32 : Amp start");
}

void loop() {
  // Toggle amp enable pin every 2 seconds
  digitalWrite(ampEnablePin, HIGH);
  Serial.println("Amp Enable Pin: HIGH (amp ON)");
  delay(2000);

  digitalWrite(ampEnablePin, LOW);
  Serial.println("Amp Enable Pin: LOW (amp OFF)");
  delay(2000);
}