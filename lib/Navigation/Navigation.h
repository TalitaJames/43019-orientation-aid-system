/**
 * @file Navigation.h
 * @author Milo
 * @brief Navigation utility functions 
 * @date 2025-10-16
 */

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <Arduino.h>
#include <math.h>
#include <GPSManager.h>
#include <DeviceRegistry.h>

/**
 * @brief Class to define the navigation utility functions necessary for interpreting GPS coordinates. 
 * 
 * Functions will include distance formula, bearings and human friendly distance. 
 * Harvesine Formula is not necessary, for minor distances, as the earths curvature will have minor effect. 
 * HOWEVER, in different locations, you can use the simple formula:
 * 
 * Earths circumference: 40075km. 
 * Distance of one latitudinal degree = 40075/360 = 111319.444m
 * Distance of one longitudinal degree = one latitudinal degree * cos(latitudinal coordinate of location)
 * 
 * @note See GPSManager.h
 */
class Navigation {

private:
    float metersPerDegreeLat;   // ~111,319.444 meters
    float metersPerDegreeLon;   // ~92,417.805 meters (at 33°S)
    float BearingData[10];
    int idx = 0;
    float lastHeading = 0.0f;

public:

    /**
     * @brief Constructor with location-specific calibration
     * @param latitude Reference latitude for calculating longitude scaling
     */
    Navigation(float referenceLatitude = -33.8688);

    /**
     * @brief Calculate the straight-line distance between two geographic coordinates. 
     */
    float distanceBetween(float lat1, float lon1, float lat2, float lon2);

    /**
     * @brief Calculate the absolute bearing from point 1 to point 2
     */
    float bearingTo(float lat1, float lon1, float lat2, float lon2);

    /**
     * @brief Calculate relative bearing from boat's perspective
     */
    float relativeBearing(float targetBearing, float boatHeading);

    /**
     * @brief Normalise angle to 0-360 range
     */
    static float normaliseAngle(float angle);

    /**
     * @brief give more reasonable answers for heading subtractions. 
     */
    float angleDifference(float angle1, float angle2);

    /**
     * @brief find boat heading compare to global heading through past gps data 
     */
    float computeHeadingTrend(const GPSHistory* points, uint8_t count);

};

#endif