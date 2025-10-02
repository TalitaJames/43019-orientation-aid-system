#include "location.h"

// array of each boat/buoy (0 is always this device)
extern Device devices[];

/**
 * @brief finds the nearest buoy and returns its address
 *
 * @return int address of the buoy as a position in the devices array
 */
int nearestBuoy(){
    int nearestAddr = 1;
    int nearestDistance = INT_MAX;

    for(size_t i = 1; i < DEVICE_COUNT; i++) // start at 1 because 0 is self
    {
        if(devices[i].type == Type::BUOY){
            int dist = distanceToDevice(i);
            if(dist - nearestDistance < 1e-4){
                nearestAddr = i;
                nearestDistance = dist;
            }
        }
    }
    return nearestAddr;
}

/**
 * @brief finds the nearest boat and returns its address
 *
 * @return int address of the buoy as a position in the devices array
 */
int nearestBoat(){
    int nearestAddr = 1;
    double nearestDistance = INT_MAX;

    for(size_t i = 1; i < DEVICE_COUNT; i++) // start at 1 because 0 is self
    {
        if(devices[i].type == Type::BOAT){
            int dist = distanceToDevice(i);
            if(dist - nearestDistance < 1e-4){
                nearestAddr = i;
                nearestDistance = dist;
            }
        }
    }
    return nearestAddr;
}

/**
 * @brief return the distance from self to the device
 *
 * @param d the device address
 * @return int the distance rounded to the nearest whole meter [m]
 */
int distanceToDevice(int d){
    double dist = TinyGPSPlus::distanceBetween(devices[0].gps.location.lat(),
                                               devices[0].gps.location.lng(),
                                               devices[d].gps.location.lat(),
                                               devices[d].gps.location.lng());
    return ROUND_2_INT(dist);
}

/**
 * @brief return the clock direction from self to the device
 *
 * @param d the device address
 * @return int clock direction, value range [1, 12]
 */
int directionToDevice(int d);

/**
 * @brief checks if another boat is in proximity
 * (defined by ALARM_PROXIMITY const)
 *
 * @return true if a boat is in proximity, else false
 */
bool checkBoatProximity(){
    for(size_t i = 1; i < DEVICE_COUNT; i++) // start at 1 because 0 is self
    {
        int dist = distanceToDevice(i);
        if(dist - ALARM_PROXIMITY < 1e-4 && devices[i].type == Type::BOAT){
            return true;
        }
    }
    return false;
}

/**
 * @brief initialises the GPS devices
 */
void setupDevices(){
    devices[0] = Device(Type::BOAT);
    devices[1] = Device(Type::BOAT);
    devices[2] = Device(Type::BUOY);
    devices[3] = Device(Type::BUOY);
}

/**
 * @brief Get new GPS information
 */
void updateDevices(){
  //TODO this needs to be fixed later to getting data from the ESP and not hardcoded
}

/**
 * @brief Takes a char stream and sends it to that GPS
 *
 * @param deviceIndex specific device
 * @param stream the stream
 */
void updateDevicesFromStream(int deviceIndex, const char *stream) {
    for (const char* p = stream; *p; p++) {
        devices[deviceIndex].gps.encode(*p);
    }
}


/**
 * @brief prints a summary of GPS information
 * @cite [TinyGPS code example](https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/BasicExample/BasicExample.ino)
 * @param gps object to get data from
 */
void displayGPSInfo(TinyGPSPlus gps)
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
