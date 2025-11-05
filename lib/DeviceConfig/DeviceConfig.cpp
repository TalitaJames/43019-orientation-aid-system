// lib/DeviceConfig/DeviceConfig.cpp
#include "DeviceConfig.h"

DeviceConfig::DeviceConfig() 
  : deviceType(DEVICE_TYPE_BOAT), deviceID(0), configured(false) {
}

bool DeviceConfig::begin() {
  prefs.begin("sailing-nav", true);  // Read-only mode
  
  // Check if configured
  configured = prefs.getBool("configured", false);
  
  if (!configured) {
    Serial.println("Device not configured - run SetDeviceID.ino first!");
    prefs.end();
    return false;
  }
  
  // Read configuration from flash
  deviceType = static_cast<DeviceType>(prefs.getUChar("deviceType", 1));
  deviceID = prefs.getUChar("deviceID", 0);
  
  prefs.end();
  
  // Validate
  if (!isValidID(deviceType, deviceID)) {
    Serial.println("Invalid device configuration in Flash!");
    configured = false;
    return false;
  }
  
  Serial.println("Device configured as: " + getDeviceName());
  return true;
}

bool DeviceConfig::setConfig(DeviceType type, uint8_t id) {
  if (!isValidID(type, id)) {
    Serial.println("Invalid device type or ID!");
    return false;
  }
  
  prefs.begin("sailing-nav", false);  // Write mode
  
  prefs.putBool("configured", true);
  prefs.putUChar("deviceType", static_cast<uint8_t>(type));
  prefs.putUChar("deviceID", id);
  
  prefs.end();
  
  deviceType = type;
  deviceID = id;
  configured = true;
  
  Serial.println("Configuration saved: " + getDeviceName());
  return true;
}

void DeviceConfig::clearConfig() {
  prefs.begin("sailing-nav", false);
  prefs.clear();
  prefs.end();
  
  configured = false;
  Serial.println("Configuration cleared");
}

String DeviceConfig::getDeviceName() const {
  String name;
  if (deviceType == DEVICE_TYPE_BOAT) {
    name = "Boat #" + String(deviceID);
  } else {
    name = "Buoy #" + String(deviceID);
  }
  return name;
}

bool DeviceConfig::isValidID(DeviceType type, uint8_t id) {
  if (type == DEVICE_TYPE_BOAT) {
    return (id >= 1 && id <= MaxBoatNumber);
  } else if (type == DEVICE_TYPE_BUOY) {
    return (id > MaxBoatNumber && id <= MaxDeviceNumber);
  }
  return false;
}