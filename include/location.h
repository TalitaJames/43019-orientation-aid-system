/**
 * @file location.h
 * @author Talita
 * @brief functions to gather location and distance information from GPS sensors
 * @date 2025-09-21
 *
 * TinyGPSPlus expects messages in NMEA format
 *      http://lefebure.com/articles/nmea/ explaining the message protocol
 *      https://nmeagen.org/ generating sample NMEA messages
 */

#ifndef LOCATION_H
#define LOCATION_H

#include <TinyGPSPlus.h>
#include "constants.h"

enum Type {BOAT, BUOY};
struct Device{
    Type type;
    TinyGPSPlus gps = TinyGPSPlus();

    Device(Type t) : type(t), gps(TinyGPSPlus()) {}
};

// array of each boat/buoy (0 is always this device)
extern Device devices[DEVICE_COUNT];

int nearestBuoy();
int nearestBoat();

int distanceToDevice(int);
int directionToDevice(int);

bool checkBoatProximity();

void setupDevices();
void updateDevices();
void updateDeviceFromStream();
void displayGPSInfo(TinyGPSPlus);


#endif //LOCATION_H