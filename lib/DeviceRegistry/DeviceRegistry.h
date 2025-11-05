/**
 * @file DeviceRegistry.h
 * @author Liem
 * @brief Saving GPS location
 * @date 2025-10-27
 */

#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H

#include <Arduino.h>
#include <DeviceConfig.h>

#define MaxDevice (DeviceConfig::MaxDeviceNumber)
#define MaxBoat (DeviceConfig::MaxBoatNumber)
#define MaxBuoy (DeviceConfig::MaxBuoyNumber)

class DeviceRegistry {
private:

  
public:
  //ig the list/array would be a struct, i dont want to add random libs
  /*
  bits from platformio ini
  [env:default]
  platform = espressif32
  board = esp32dev
  framework = arduino
  lib_deps = 
    mikalhart/TinyGPSPlus@^1.1.0
    jdollar/SoftwareSerialEsp32@0.0.0-alpha+sha.6d373ecd5f
    jgromes/RadioLib@^7.3.0
    sandeepmistry/LoRa@^0.8.0
  monitor_speed = 115200
  */
  //This class shouls have function to:
  //and these arrays should be local for each device and not shared

  //save gps reading from the last few seconds, lets say 10s, every 1s into a list/array
  // so it can be used to calculate the heading of the boat in another class
  //input is lat lon
  //this list should only have lat and lon for each entry. Every 1s, when new data is received
  //remove the oldest entry and push in the new one

  //save info of boat devices into a list/array. input is id, distance
  //when getting new info, it should update entry if they have same id, add new entry if new id

  //save info of buoy devices into a list/array. input is id, distance, heading
  //when getting new info, it should update entry if they have same id, add new entry if new id

};

#endif