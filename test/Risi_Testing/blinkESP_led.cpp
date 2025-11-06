#include <Arduino.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2   // Most ESP32 DevKits use GPIO2 for the onboard LED
#endif

void printResetReason() {
  esp_reset_reason_t r = esp_reset_reason();
  Serial.print("Reset reason: "); Serial.println((int)r);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Serial is optional; code runs fine without USB connected.
  Serial.begin(115200);
  delay(50);
  Serial.println("\nBoot OK");
  printResetReason();
}

void loop() {
  // Blink fast; if it ever pauses or restarts, that hints at brownout/reset
  digitalWrite(LED_BUILTIN, HIGH);
  delay(150);
  digitalWrite(LED_BUILTIN, LOW);
  delay(150);

  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last += 1000;
    if (Serial) Serial.println("Heartbeat");
  }
}
