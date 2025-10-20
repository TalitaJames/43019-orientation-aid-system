/**
 * @file GPSManager.h
 * @author Milo
 * @brief GPS interface using TinyGPSPlus
 * @date 2025-10-20
 */

#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>


//GPS UART configuration.
#define GPS_RX_PIN  32
#define GPS_TX_PIN  33
#define GPS_BAUD    9600

class GPSManager {
private:
  TinyGPSPlus gps;
  HardwareSerial* gpsSerial;
  
  // Cache last valid position
  float lastLat;
  float lastLon;
  uint16_t lastHeading;
  uint16_t lastSpeed;  // cm/s
  uint32_t lastGPSTime;
  bool hasValidFix;
  
public:

  //Initialise all member variables to safe defaults. 
  GPSManager();

  //deletes loaded HardwareSerial Object. 
  ~GPSManager();
  
  /**
   * @brief Create HarwdareSerial Object. 
   * uses defined pins for uart configuration above. 
   */
  bool begin();
  
  // Update GPS (call in loop)
  /**
   * @brief Read available bytes and update GPS values. 
   */
  void update();
  
  // Get current position
  bool getPosition(float& lat, float& lon);
  
  // Get current heading (0-359)
  uint16_t getHeading();
  
  // Get current speed (cm/s)
  uint16_t getSpeed();
  
  // Get GPS time in milliseconds since midnight UTC
  uint32_t getGPSTimeMillis();
  
  // Check if we have valid fix
  bool hasFixAndTime() const { return hasValidFix && gps.time.isValid(); }
  bool hasFix() const { return hasValidFix; }
  
  // Get number of satellites
  uint8_t getSatellites();
  
  // Get raw TinyGPS object for advanced use
  TinyGPSPlus& getRawGPS() { return gps; }
};

#endif