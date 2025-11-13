/**
 * @file DeviceConfig.h
 * @author Milo
 * @brief Defining the packet structures that are transmitted. 
 * @date 2025-10-13
 * 
 * Features Preferences Library, a library used to save data on flash memory. This is important for the devices to permanently remember their ID.
 * https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
 *
 */

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <Preferences.h>
#include <Arduino.h>
#include <Protocol.h>

// EEPROM memory map
#define EEPROM_MAGIC_ADDR     0    // 2 bytes - magic number (0xABCD)
#define EEPROM_DEVICE_TYPE    2    // 1 byte - device type
#define EEPROM_DEVICE_ID      3    // 1 byte - device ID
#define EEPROM_SIZE           4

#define CONFIG_MAGIC_NUMBER   0xABCD

class DeviceConfig {
// Changeable config value
public:
  static constexpr int MaxBoatNumber = 8;
  static constexpr int MaxBuoyNumber = 2;
  static constexpr int MaxDeviceNumber = MaxBoatNumber + MaxBuoyNumber;
  static constexpr int BUTTON_PIN = 12;
  static constexpr int PPS_PIN = 27;
  static constexpr int SIREN_PIN = 5;
  static constexpr float siren_dist = 30.0f;
  static constexpr float DANGER_DISTANCE_M = 20.0f;

private:
  DeviceType deviceType; //cached in RAM for fast access
  uint8_t deviceID; //cached in RAM for fast access
  bool configured; //checks if device is configured
  Preferences prefs; //flash storage interface
  
public:
  /**
   * @brief Construct a new DeviceConfig object.
   */
  DeviceConfig();
  
  /**
   * @brief Initializes Preferences and loads stored configuration from flash.
   * 
   * @return true if valid configuration was found and loaded, false otherwise.
   */
  bool begin();
  
  /**
   * @brief Programs and saves the device configuration to flash.
   * 
   * Should typically be called only once per device.
   * 
   * @param type The device type (e.g., boat, buoy).
   * @param id   The unique ID for the device.
   * @return true if configuration was successfully saved, false otherwise.
   */
  bool setConfig(DeviceType type, uint8_t id);
  
  /**
   * @brief Gets the current device type.
   * @return DeviceType The configured device type.
   */
  DeviceType getDeviceType() const { return deviceType; }
  
  /**
   * @brief Gets the current device ID.
   * @return uint8_t The configured device ID.
   */
  uint8_t getDeviceID() const { return deviceID; }
  
  /**
   * @brief Checks whether valid configuration is loaded.
   * @return true if configuration is valid, false otherwise.
   */
  bool isConfigured() const { return configured; }
  
  /**
   * @brief Clears all stored configuration data from flash.
   */
  void clearConfig();
  
  /**
   * @brief Returns a human-readable device name based on its type and ID.
   * 
   * Example: "Boat #3" or "Buoy #1".
   * 
   * @return String Descriptive name of the device.
   */
  String getDeviceName() const;
  
  /**
   * @brief Validates a given device ID based on the device type.
   * 
   * Ensures that the ID falls within acceptable range for the specified type.
   * 
   * @param type The device type to validate.
   * @param id The device ID to check.
   * @return true if ID is valid for the given type, false otherwise.
   */
  static bool isValidID(DeviceType type, uint8_t id);
};

#endif