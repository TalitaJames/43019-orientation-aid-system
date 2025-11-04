#include "Navigation.h"

Navigation::Navigation(float referenceLatitude) {
  // One degree of latitude is always ~111,319.444 meters
  metersPerDegreeLat = 111319.444;
  
  // One degree of longitude varies by latitude: 111,319.444 * cos(latitude)
  metersPerDegreeLon = 111319.444 * cos(radians(abs(referenceLatitude)));
}

float Navigation::distanceBetween(float lat1, float lon1, float lat2, float lon2) {

    //change in x is longitude
    //change in y is latitude 
    float deltaLat = lat2 - lat1;
    float deltaLon = lon2 - lon1;
  
    // Convert to meters using location-specific scaling
    float deltaLatMeters = deltaLat * metersPerDegreeLat;
    float deltaLonMeters = deltaLon * metersPerDegreeLon;

    // Pythagorean theorem: distance = sqrt(x² + y²)
    float distance = sqrt(pow(deltaLatMeters, 2) + pow(deltaLonMeters, 2));


    return distance;
}

float Navigation::bearingTo(float lat1, float lon1, float lat2, float lon2) {
    // Calculate differences
    float deltaLat = lat2 - lat1;
    float deltaLon = lon2 - lon1;

    // Convert to meters
    float deltaLatMeters = deltaLat * metersPerDegreeLat;
    float deltaLonMeters = deltaLon * metersPerDegreeLon;

    // Calculate bearing using atan2
    // atan2(x, y) gives angle from north (0°), measuring clockwise
    float bearing = degrees(atan2(deltaLonMeters, deltaLatMeters));

    // Normalize to 0-360
    return normaliseAngle(bearing);
}

float Navigation::relativeBearing(float targetBearing, float boatHeading) {
  // Calculate difference between target bearing and boat heading
  float relative = angleDifference(boatHeading, targetBearing);
  
  return relative;
}


float Navigation::normaliseAngle(float angle) {
  // Normalize angle to 0-360 range
  while (angle < 0) {
    angle += 360;
  }
  while (angle >= 360) {
    angle -= 360;
  }
  return angle;
}

float Navigation::angleDifference(float angle1, float angle2) {
  // Calculate shortest difference between two angles
  // Result is -180 to +180
  
  float diff = angle2 - angle1;
  
  // Normalize to -180 to +180
  while (diff > 180) {
    diff -= 360;
  }
  while (diff < -180) {
    diff += 360;
  }
  
  return diff;
}


float* Navigation::computeHeadingTrend(const GPSDataPoint* points, uint8_t count) {
  if (count < 2) return nullptr;

  uint8_t validPairs = 0;

  for (uint8_t i = 0; i < count - 1; i++) {
    float lat1 = points[i].lat;
    float lon1 = points[i].lon;
    float lat2 = points[i + 1].lat;
    float lon2 = points[i + 1].lon;

    BearingData[idx] = bearingTo(lat1, lon1, lat2, lon2);
    idx = (idx + 1) % sizeof(BearingData);
    validPairs++;
  }

  if (validPairs == 0) return nullptr;

  return BearingData;
}

