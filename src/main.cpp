// #include <Arduino.h>
// #include <SPI.h>
// #include <LoRa.h>
// #include <HardwareSerial.h>
// #include <TinyGPSPlus.h>

// // ---------- LoRa pins (BOAT) ----------
// #define LORA_SCK   18
// #define LORA_MISO  19
// #define LORA_MOSI  23
// #define LORA_CS     5       // BOAT keeps CS on 5
// #define LORA_RST   14
// #define LORA_IRQ   26       // DIO0

// // ---------- GPS (UART2) ----------
// HardwareSerial SerialGPS(2);           // UART2
// TinyGPSPlus gps;
// const int GPS_RX = 32;                 // ESP32 RX2  ← GPS TX
// const int GPS_TX = 33;                 // ESP32 TX2  → GPS RX (optional)

// // ---------- Audio / Talkie ----------
// #include "Talkie.h"
// #include "Vocab_US_Large.h"
// #include "Vocab_Special.h"
// Talkie voice;
// const int BUTTON = 12;                 // push button to GND (INPUT_PULLUP)

// // ---- small helpers (Talkie) ----
// void sayPause() { voice.say(spPAUSE1); }
// void sayDigit(int d){
//   static const uint8_t* n[] = {
//     sp2_ZERO, sp2_ONE, sp2_TWO, sp2_THREE, sp2_FOUR,
//     sp2_FIVE, sp2_SIX, sp2_SEVEN, sp2_EIGHT, sp2_NINE
//   }; voice.say(n[d]);
// }
// void sayDigits(int x){
//   if (x == 0){ sayDigit(0); return; }
//   int buf[6], i=0; while (x>0 && i<6){ buf[i++]=x%10; x/=10; }
//   for (int k=i-1;k>=0;--k){ sayDigit(buf[k]); sayPause(); }
// }
// void sayClockHour(int h){
//   switch(h){
//     case 1: voice.say(sp2_ONE);break;   case 2: voice.say(sp2_TWO);break;
//     case 3: voice.say(sp2_THREE);break; case 4: voice.say(sp2_FOUR);break;
//     case 5: voice.say(sp2_FIVE);break;  case 6: voice.say(sp2_SIX);break;
//     case 7: voice.say(sp2_SEVEN);break; case 8: voice.say(sp2_EIGHT);break;
//     case 9: voice.say(sp2_NINE);break;  case 10: voice.say(sp2_TEN);break;
//     case 11: voice.say(sp2_ELEVEN);break; case 12: voice.say(sp2_TWELVE);break;
//   } voice.say(sp3_OCLOCK);
// }
// void speakDemo(){
//   voice.say(sp4_EXPECT); sayPause();
//   voice.say(sp4_TARGET); sayPause();     // stand-in for “BUOY”
//   sayClockHour(3); sayPause();           // “THREE O’CLOCK”
//   sayDigits(1); sayDigits(8); sayDigits(0);
//   voice.say(sp2_METER);
// }

// // ---------- setup ----------
// void setup() {
//   Serial.begin(115200);
//   delay(400);

//   pinMode(BUTTON, INPUT_PULLUP);
  

//   // GPS
//   SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

//   // LoRa
//   SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
//   LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
//   if (!LoRa.begin(915E6)) {
//     Serial.println("LoRa init failed"); while (1) delay(1000);
//   }
//   Serial.println("BOAT up: GPS→LoRa, Talkie on button.");
// }

// // ---------- loop ----------
// void loop() {
//   // 1) Button → speak once
//   static int lastBtn = HIGH;
//   int b = digitalRead(BUTTON);
//   if (lastBtn == HIGH && b == LOW) {     // pressed
//     Serial.println("Button: speak");
//     speakDemo();
//     delay(250);                          // crude debounce
//   }
//   lastBtn = b;

//   // 2) Read GPS stream
//   while (SerialGPS.available()) gps.encode(SerialGPS.read());

//   static uint32_t t0=0, pkt=0;
//   if (millis()-t0 > 2000) {
//     t0 = millis(); pkt++;

//     double lat = gps.location.isValid() ? gps.location.lat() : 0.0;
//     double lon = gps.location.isValid() ? gps.location.lng() : 0.0;
//     uint32_t sats = gps.satellites.value();
//     float hdop = gps.hdop.isValid() ? gps.hdop.value()/100.0f : 99.99f;

//     // --- add ID and DESC fields ---
//     String msg = "ID=BOAT"
//                  ",DESC=BoatTest"
//                  ",LAT=" + String(lat,6) +
//                  ",LON=" + String(lon,6) +
//                  ",SATS=" + String(sats) +
//                  ",HDOP=" + String(hdop,2) +
//                  ",N=" + String(pkt);

//     Serial.println("TX: " + msg);
//     LoRa.beginPacket();
//     LoRa.print(msg);
//     LoRa.endPacket();
//   }
// }


#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

Talkie voice;

const int buttonPin = 12;
bool lastButton = HIGH;


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
  sayClockHour(hour1to12);
  sayPause();
}

void sayRangeDigits(int meters) {
  sayDigits(meters);
}

void sayMeters(int m) {
  sayDigits(m);
  voice.say(sp2_METER);
}

// --- demo ---
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  bool current = digitalRead(buttonPin);

  // Detect button press (active LOW)
  if (lastButton == HIGH && current == LOW) {
    Serial.println("Button pressed, speaking...");
    sayExpectBuoy();
    sayBearingClockStyle(3);   // Example "3 O'CLOCK"
    sayMeters(180);            // Example "180 METERS"
    delay(500);                // debounce
  }

  lastButton = current;
}
