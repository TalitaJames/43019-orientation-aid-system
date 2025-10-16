#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_CS   5
#define LORA_RST  14
#define LORA_IRQ  26

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Module Test Starting...");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(915E6)) { // 915 MHz for AU/US
    Serial.println("❌ LoRa init failed. Check wiring or SPI pins!");
    while (true);
  }
  
  Serial.println("✅ LoRa init succeeded!");
  Serial.print("Frequency: ");
  Serial.println("LoRa started at 915 MHz");
}

void loop() {
  Serial.println("Sending test packet...");
  LoRa.beginPacket();
  LoRa.print("Hello from ESP32 LoRa!");
  LoRa.endPacket();
  
  delay(5000);
}

















