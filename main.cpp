#include "Lora.h"

#define LORA_SCK   18 // connect 5v and gnd for lora and gps too
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_CS    5
#define LORA_RST   26
#define LORA_IRQ   27 //G0 pin

#define GPS_RX     16
#define GPS_TX     17

LoraGPS loraGPS;

void setup() {
    loraGPS.begin();  // just call begin, pins are defined here
}

void loop() {
    loraGPS.update();
}