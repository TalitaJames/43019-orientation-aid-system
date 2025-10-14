// #include <Arduino.h>

// void setup() {


// }

// void loop() {

  
// }

// #include <SPI.h>
// #include <LoRa.h>

// #define LORA_SCK  18
// #define LORA_MISO 19
// #define LORA_MOSI 23
// #define LORA_CS   5
// #define LORA_RST  14
// #define LORA_IRQ  26

// void setup() {
//   Serial.begin(115200);
//   while (!Serial);

//   Serial.println("LoRa Module Test Starting...");

//   SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
//   LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

//   if (!LoRa.begin(915E6)) { // 915 MHz for AU/US
//     Serial.println("❌ LoRa init failed. Check wiring or SPI pins!");
//     while (true);
//   }
  
//   Serial.println("✅ LoRa init succeeded!");
//   Serial.print("Frequency: ");
//   Serial.println("LoRa started at 915 MHz");
// }

// void loop() {
//   Serial.println("Sending test packet...");
//   LoRa.beginPacket();
//   LoRa.print("Hello from ESP32 LoRa!");
//   LoRa.endPacket();
  
//   delay(5000);
// }


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

#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

Talkie voice;

// --- helpers ---
void sayPause() { voice.say(spPAUSE1); }

void sayDigit(int d) {
  static const uint8_t* nums[] = {
    sp2_ZERO, sp2_ONE, sp2_TWO, sp2_THREE, sp2_FOUR,
    sp2_FIVE, sp2_SIX, sp2_SEVEN, sp2_EIGHT, sp2_NINE
  };
  voice.say(nums[d]);
}

void sayDigits(int n) {
  if (n == 0) { sayDigit(0); return; }
  int buf[6], i=0;
  while (n > 0 && i < 6) { buf[i++] = n % 10; n /= 10; }
  for (int k=i-1; k>=0; --k) { sayDigit(buf[k]); voice.say(spPAUSE1); }
}

void sayClockHour(int h) {
  // 1..12 only
  switch (h) {
    case 1: voice.say(sp2_ONE); break;
    case 2: voice.say(sp2_TWO); break;
    case 3: voice.say(sp2_THREE); break;
    case 4: voice.say(sp2_FOUR); break;
    case 5: voice.say(sp2_FIVE); break;
    case 6: voice.say(sp2_SIX); break;
    case 7: voice.say(sp2_SEVEN); break;
    case 8: voice.say(sp2_EIGHT); break;
    case 9: voice.say(sp2_NINE); break;
    case 10: voice.say(sp2_TEN); break;
    case 11: voice.say(sp2_ELEVEN); break;
    case 12: voice.say(sp2_TWELVE); break;
  }
  voice.say(sp3_OCLOCK);
}

// --- phrases ---
void sayExpectBuoy() {
  voice.say(sp4_EXPECT);   // "EXPECT"
  sayPause();
  voice.say(sp2_GALLONS);  // stand-in for "BUOY"
  sayPause();
}

void sayBearingClockStyle(int hour1to12) {
  sayClockHour(hour1to12); // e.g. "THREE O'CLOCK"
  sayPause();
}

void sayRangeDigits(int meters) {
  // e.g. "ONE EIGHT ZERO" (no 'meters' token available here)
  sayDigits(meters);
}

// NEW: speak "<digits> meter"
void sayMeters(int m) {
  sayDigits(m);          // from earlier helper
  voice.say(sp2_METER);  // your token
}

// --- demo ---
void setup() { }

void loop() {
  sayExpectBuoy();

  // e.g. bearing "THREE O'CLOCK"
  sayBearingClockStyle(3);

  // e.g. distance/bearing value as digits (180)

  sayMeters(180);

  delay(3000);
}

