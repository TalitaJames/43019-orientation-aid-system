#include <Arduino.h>

const int AMP_EN = 4;    // shutdown/enable pin
const int AUDIO = 25;    // DAC1 output → RIN

void setup() {
  pinMode(AMP_EN, OUTPUT);
  digitalWrite(AMP_EN, HIGH);  // enable amp
}

void loop() {
  // beep for 1s
  unsigned long endTime = millis() + 1000;
  while (millis() < endTime) {
    dacWrite(AUDIO, 129);  // barely above mid
    delayMicroseconds(500);
    dacWrite(AUDIO, 127);  // barely below mid
    delayMicroseconds(500);
  }

  // silence for 1s
  dacWrite(AUDIO, 128);   // mid-level = quiet
  delay(1000);
}