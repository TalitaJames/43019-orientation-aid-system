// ConfigTool/SetDeviceID.ino
#include <DeviceConfig.h>
#include <Arduino.h>

DeviceConfig config;

void configureBoat() {
  Serial.println("\n--- BOAT CONFIGURATION ---");
  Serial.println("Enter boat number (1-20):");
  
  while (!Serial.available()) { delay(10); }
  int id = Serial.parseInt();
  
  if (id < 1 || id > 20) {
    Serial.println("Invalid ID! Must be 1-20");
    return;
  }
  
  if (config.setConfig(DEVICE_TYPE_BOAT, id)) {
    Serial.println("\nSuccess! Device configured as: " + config.getDeviceName());
    Serial.println("Please restart and upload BoatDevice.ino");
  }
}

void configureBuoy() {
  Serial.println("\n--- BUOY CONFIGURATION ---");
  Serial.println("Enter buoy number:");
  Serial.println("  21 = Windward Mark");
  Serial.println("  22 = Finish Buoy");
  
  while (!Serial.available()) { delay(10); }
  int id = Serial.parseInt();
  
  if (id < 21 || id > 22) {
    Serial.println("Invalid ID! Must be 21 or 22");
    return;
  }
  
  if (config.setConfig(DEVICE_TYPE_BUOY, id)) {
    Serial.println("\nSuccess! Device configured as: " + config.getDeviceName());
    Serial.println("Please restart and upload BuoyDevice.ino");
  }
}

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
  Serial.println("  1. Configure as BOAT (ID 1-20)");
  Serial.println("  2. Configure as BUOY (ID 21-22)");
  Serial.println("  3. Clear configuration");
  Serial.println("=================================\n");
}

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