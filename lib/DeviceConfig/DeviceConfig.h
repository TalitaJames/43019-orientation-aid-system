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
private:
  DeviceType deviceType; //cached in RAM for fast access
  uint8_t deviceID; //cached in RAM for fast access
  bool configured; //checks if device is configured
  Preferences prefs; //flash storage interface
  
public:
  DeviceConfig();
  
  // Initialize and load from Flash
  bool begin();
  
  // Set device configuration (call once to program device)
  bool setConfig(DeviceType type, uint8_t id);
  
  // Get device info
  DeviceType getDeviceType() const { return deviceType; }
  uint8_t getDeviceID() const { return deviceID; }
  bool isConfigured() const { return configured; }
  
  // Clear configuration
  void clearConfig();
  
  // Get human-readable name
  String getDeviceName() const;
  
  // Validate ID range
  static bool isValidID(DeviceType type, uint8_t id);
};

#endif