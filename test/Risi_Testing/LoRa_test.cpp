//////////   DUMP REGISTERS   ///////////////

// #include <SPI.h>
// #include <LoRa.h>

// #define LORA_SCK  18
// #define LORA_MISO 19
// #define LORA_MOSI 23
// #define LORA_CS    5
// #define LORA_RST  14
// #define LORA_IRQ  26

// void setup() {
//   Serial.begin(115200);
//   while (!Serial);
//   Serial.println("LoRa Dump Registers (ESP32)");

//   SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
//   LoRa.setSPI(SPI);
//   LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

//   if (!LoRa.begin(915E6)) {
//     Serial.println("❌ LoRa init failed. Check wiring.");
//     while (true);
//   }

//   Serial.println("✅ LoRa init succeeded!");
//   LoRa.dumpRegisters(Serial);
// }

// void loop() {}

/////////////////////// sending packets test ///////////////

// #include <Arduino.h>
// #include <SPI.h>
// #include <LoRa.h>

// #define LORA_SCK   18
// #define LORA_MISO  19
// #define LORA_MOSI  23
// #define LORA_CS     5
// #define LORA_RST   14
// #define LORA_IRQ   26  // DIO0

// void setup() {
//   Serial.begin(115200);
//   delay(1500);  // wait for serial monitor to attach

//   Serial.println("\n=== LoRa TX Test ===");
//   SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
//   LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

//   if (!LoRa.begin(915E6)) {
//     Serial.println("❌ LoRa init failed — check wiring and power.");
//     while (true) delay(1000);
//   }

//   Serial.println("✅ LoRa init OK at 915 MHz!");
//   Serial.println("Now sending test packets every 5 seconds...\n");
// }

// void loop() {
//   static int counter = 0;
//   counter++;

//   Serial.print("Sending packet #");
//   Serial.println(counter);

//   LoRa.beginPacket();
//   LoRa.print("Hello from ESP32! Packet #");
//   LoRa.print(counter);
//   LoRa.endPacket();

//   delay(5000);
// }