#include <Arduino.h>
#include "location.h"


void setup(){
  Serial.begin(9600);
  setupDevices();

  Serial.println(F("setup done"));
}


void loop(){
    delay(2000);
    Serial.println("\n\nRUN:");
    updateDevices();

    for (size_t i = 0; i < DEVICE_COUNT; i++){
        Serial.print("\t");
        displayGPSInfo(devices[i].gps);
    }

    int nearBuoy = nearestBuoy();
    int nearBoat = nearestBoat();

    // Serial.println("\n");
    Serial.println(nearBuoy +" is the closest Buoy");
    Serial.println(nearBoat +" is the closest Boat");

    if (checkBoatProximity()){
        Serial.print("A boat is in proximity");
    }
}