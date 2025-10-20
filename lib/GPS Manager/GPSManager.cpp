// lib/GPSManager/GPSManager.cpp
#include "GPSManager.h"

GPSManager::GPSManager() 
  : gpsSerial(nullptr), lastLat(0), lastLon(0), 
    lastHeading(0), lastSpeed(0), lastGPSTime(0), hasValidFix(false) {
}

GPSManager::~GPSManager() {
  if (gpsSerial) {
    delete gpsSerial;
  }
}

bool GPSManager::begin() {
  gpsSerial = new HardwareSerial(2); //UART2
  gpsSerial->begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  Serial.println("GPS initialized, waiting for fix...");
  return true;
}

void GPSManager::update() {
  while (gpsSerial->available() > 0) {
    char c = gpsSerial->read();
    gps.encode(c);
  }
  
  // Update cached values if we have valid data. Caching is useful to give you instant access without checking TinyGPSPlus every time. 
  if (gps.location.isValid() && gps.location.isUpdated()) {
    lastLat = gps.location.lat();
    lastLon = gps.location.lng();
    hasValidFix = true;
  }
  
  //cache course (which direction the boat is going based on velocity. different to IMU reading).
  if (gps.course.isValid() && gps.course.isUpdated()) {
    lastHeading = (uint16_t)gps.course.deg();
  }
  
  //cache speed
  if (gps.speed.isValid() && gps.speed.isUpdated()) {
    // Convert knots to cm/s (1 knot = 51.4444 cm/s)
    lastSpeed = (uint16_t)(gps.speed.knots() * 51.4444);
  }
  
  //cache time 
  if (gps.time.isValid() && gps.time.isUpdated()) {
    lastGPSTime = (gps.time.hour() * 3600000UL) +
                  (gps.time.minute() * 60000UL) +
                  (gps.time.second() * 1000UL) +
                  (gps.time.centisecond() * 10UL);
  }
}

bool GPSManager::getPosition(float& lat, float& lon) {
  if (hasValidFix) {
    lat = lastLat;
    lon = lastLon;
    return true;
  }
  return false;
}

uint16_t GPSManager::getHeading() {
  return lastHeading;
}

uint16_t GPSManager::getSpeed() {
  return lastSpeed;
}

uint32_t GPSManager::getGPSTimeMillis() {
  return lastGPSTime;
}

uint8_t GPSManager::getSatellites() {
  if (gps.satellites.isValid()) {
    return gps.satellites.value();
  }
  return 0;
}