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

float Navigation::computeHeadingTrend(const GPSHistory* points, uint8_t count) {
  if (count < 2) return lastHeading;  // not enough data

  float totalBearing = 0.0f;
  uint8_t validCount = 0;

  // Pull gps data from history array
  for (uint8_t i = 0; i < count - 1; i++) {
    float lat1 = points[i].latitude;
    float lon1 = points[i].longitude;
    float lat2 = points[i + 1].latitude;
    float lon2 = points[i + 1].longitude;

    float dist = distanceBetween(lat1, lon1, lat2, lon2);

    // Ignore small movements (e.g., GPS noise)
    if (dist < 1.5f) continue;

    // Find bearing to between 2 points
    float bearing = bearingTo(lat1, lon1, lat2, lon2);
    totalBearing += bearing;
    validCount++;
  }

  if (validCount == 0) {
    // Boat stationary — keep last known heading
    return lastHeading;
  }

  // Averageing bearing to get a consistent reading
  float avgBearing = totalBearing / validCount;

  // Normalize heading to 0-360
  lastHeading = normaliseAngle(avgBearing);
  return lastHeading;
}


