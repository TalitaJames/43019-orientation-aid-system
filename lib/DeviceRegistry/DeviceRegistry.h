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

#define MAX_HISTORY_SEC 10   // how many seconds of GPS history to keep
#define MAX_DEVICE       (DeviceConfig::MaxDeviceNumber)
#define MAX_BOAT         (DeviceConfig::MaxBoatNumber)
#define MAX_BUOY         (DeviceConfig::MaxBuoyNumber)
#define INVALID_ID       255

struct GPSHistory {
  float latitude;
  float longitude;
  uint32_t timestamp;
};

struct BoatInfo {
  uint8_t id = INVALID_ID;
  float distance = 0.0f;
};

struct BuoyInfo {
  uint8_t id = INVALID_ID;
  float distance = 0.0f;
  uint16_t heading = -1;
};

class DeviceRegistry {
private:
  // Local GPS history buffer (FIFO)
  GPSHistory history[MAX_HISTORY_SEC];
  uint8_t historyIndex = 0;
  uint8_t historyCount = 0;

  // Local lists for other devices
  BoatInfo boats[MAX_BOAT];
  BuoyInfo buoys[MAX_BUOY];

public:
  DeviceRegistry();

  // ===== GPS History =====
  void addGPSHistory(float lat, float lon, uint32_t ts);
  const GPSHistory* getGPSHistoryArray();
  uint8_t getHistoryCount();

  // ===== Boat Management =====
  void updateBoat(uint8_t id, float distance);
  int findBoat(uint8_t id);
  float getBoatDistance(uint8_t id);

  // ===== Buoy Management =====
  void updateBuoy(uint8_t id, float distance, float heading);
  int findBuoy(uint8_t id);
  float getBuoyID(uint8_t id);
  float getBuoyDistance(uint8_t id);
  float getBuoyHeading(uint8_t id);
};

#endif