#include "LoRaModule.h"
#include <TinyGPSPlus.h>

TinyGPSPlus gps;
LoRaModule lora(gps);

void setup() {
    lora.begin();
}

void loop() {
    lora.update();
    delay(3000);
}