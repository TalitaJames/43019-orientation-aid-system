/**
 * SetDevice should be run when setting up a completely new device
 * After uploading the setdevice environment, open Serial Monitor
 * If needed, press reset on the Esp32
 * The program will ask for device type and ID
 * Type the number corresponding to your choice
 * On completion, Boat/Buoy environment can be uploaded
 * The Type and ID will be remembered
 * 
 * Note: number of devices can be set in DeviceConfig.h and it will be updated here accordingly6366
 */

// ConfigTool/SetDeviceID.ino
#include <DeviceConfig.h>
#include <Arduino.h>

// Get variable from Device Config
DeviceConfig config;
const int MaxBoat = config.MaxBoatNumber;
const int MaxBuoy = config.MaxBuoyNumber;
const int MaxDevice = MaxBoat+MaxBuoy;

// Procedure if device type boat was chosen
void configureBoat() {
  Serial.println("\n--- BOAT CONFIGURATION ---");
  Serial.println("Enter boat number (1-");
  Serial.print(MaxBoat);
  Serial.print("):");
  
  while (!Serial.available()) { delay(10); }
  int id = Serial.parseInt();
  
  if (id < 1 || id > MaxBoat) {
    Serial.println("Invalid ID! Must be 1-");
    Serial.print(MaxBoat);
    return;
  }
  
  if (config.setConfig(DEVICE_TYPE_BOAT, id)) {
    Serial.println("\nSuccess! Device configured as: " + config.getDeviceName());
    Serial.println("Please restart and upload BoatDevice.ino");
  }
}

// Procedure if device type buoy was chosen
void configureBuoy() {
  Serial.println("\n--- BUOY CONFIGURATION ---");
  Serial.println("Enter buoy number:");
  Serial.print(MaxBoat+1);
  Serial.print("-");
  Serial.print(MaxDevice);
  
  while (!Serial.available()) { delay(10); }
  int id = Serial.parseInt();
  
  if (id <= MaxBoat || id > MaxDevice) {
    Serial.println("Invalid ID! Must be ");
    Serial.print(MaxBoat+1);
    Serial.print("-");
    Serial.print(MaxDevice);
    return;
  }
  
  if (config.setConfig(DEVICE_TYPE_BUOY, id)) {
    Serial.println("\nSuccess! Device configured as: " + config.getDeviceName());
    Serial.println("Please restart and upload BuoyDevice.ino");
  }
}

// Start configuration by choosing device type
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=================================");
  Serial.println("   DEVICE CONFIGURATION TOOL");
  Serial.println("=================================\n");
  
  config.begin();
  
  Serial.println("Current configuration:");
  if (config.isConfigured()) {
    Serial.println("  Type: " + String(config.getDeviceType() == DEVICE_TYPE_BOAT ? "BOAT" : "BUOY"));
    Serial.println("  ID: " + String(config.getDeviceID()));
    Serial.println("  Name: " + config.getDeviceName());
  } else {
    Serial.println("  Not configured");
  }
  
  Serial.println("\n=================================");
  Serial.println("Choose an option:");
  Serial.println("  1. Configure as BOAT");
  Serial.println("  2. Configure as BUOY");
  Serial.println("  3. Clear configuration");
  Serial.println("=================================\n");
}

// Process input
void loop() {
  if (Serial.available()) {
    char choice = Serial.read();
    
    if (choice == '1') {
      configureBoat();
    } else if (choice == '2') {
      configureBuoy();
    } else if (choice == '3') {
      config.clearConfig();
      Serial.println("\nConfiguration cleared!");
      Serial.println("Please restart to reconfigure.");
    } else {
      Serial.println("\nInvalid input, try again");
    }
  }
}