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
