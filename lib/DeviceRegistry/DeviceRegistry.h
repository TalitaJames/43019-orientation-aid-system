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

/** @brief Stores a single GPS data point. */
struct GPSHistory {
  float latitude;
  float longitude;
  uint32_t timestamp;
};

/** @brief Stores distance info for a boat device. */
struct BoatInfo {
  uint8_t id = INVALID_ID;
  float distance = 0.0f;
};

/** @brief Stores distance and heading info for a buoy device. */
struct BuoyInfo {
  uint8_t id = INVALID_ID;
  float distance = 0.0f;
  uint16_t heading = -1;
};

/**
 * @class DeviceRegistry
 * @brief Handles local GPS history and data from nearby devices.
 */
class DeviceRegistry {
private:
  // ===== Local GPS history =====
  GPSHistory history[MAX_HISTORY_SEC];
  uint8_t historyIndex = 0;
  uint8_t historyCount = 0;

  // ===== Remote device lists =====
  BoatInfo boats[MAX_BOAT];
  BuoyInfo buoys[MAX_BUOY];

public:
  /**
   * @brief Construct a new DeviceRegistry object.
   */
  DeviceRegistry();

  // ===== GPS History =====

  /**
   * @brief Add a new GPS entry to the history buffer.
   * @param lat Latitude in decimal degrees.
   * @param lon Longitude in decimal degrees.
   * @param ts Timestamp in milliseconds.
   */
  void addGPSHistory(float lat, float lon, uint32_t ts);

  /**
   * @brief Get a pointer to the GPS history array.
   * @return Pointer to GPSHistory array.
   */
  const GPSHistory* getGPSHistoryArray();

  /**
   * @brief Get the number of valid GPS history entries.
   * @return Number of entries stored.
   */
  uint8_t getHistoryCount();

  // ===== Boat Management =====

  /**
   * @brief Update or add a boat entry.
   * @param id Boat ID.
   * @param distance Distance in meters.
   */
  void updateBoat(uint8_t id, float distance);

  /**
   * @brief Find a boat entry by ID.
   * @param id Boat ID.
   * @return Index of the boat, or -1 if not found.
   */
  int findBoat(uint8_t id);

  /**
   * @brief Get distance for a given boat ID.
   * @param id Boat ID.
   * @return Distance in meters, or 0.0 if not found.
   */
  float getBoatDistance(uint8_t id);

  // ===== Buoy Management =====

  /**
   * @brief Update or add a buoy entry.
   * @param id Buoy ID.
   * @param distance Distance in meters.
   * @param heading Heading in degrees.
   */
  void updateBuoy(uint8_t id, float distance, float heading);

  /**
   * @brief Find a buoy entry by ID.
   * @param id Buoy ID.
   * @return Index of the buoy, or -1 if not found.
   */
  int findBuoy(uint8_t id);

  /**
   * @brief Get distance for a given buoy ID.
   * @param id Buoy ID.
   * @return Distance in meters, or 0.0 if not found.
   */
  float getBuoyDistance(uint8_t id);

  /**
   * @brief Get heading for a given buoy ID.
   * @param id Buoy ID.
   * @return Heading in degrees, or -1 if not found.
   */
  float getBuoyHeading(uint8_t id);
};

#endif