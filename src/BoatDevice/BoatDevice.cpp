// Include libraries
#include <Arduino.h>
#include <DeviceConfig.h>
#include <DeviceRegistry.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>
#include <Button.h>
#include <TTS.h>

// Definitions
#define MaxDevice (DeviceConfig::MaxDeviceNumber) // 10 devices maximum
#define MaxBoat (DeviceConfig::MaxBoatNumber) // 8 boats
#define MaxBuoy (DeviceConfig::MaxBuoyNumber) // 2 buoys. this might not even need to be included in the boat executable.
#define PPS_PIN 27 // PPS pin. used for interrupts. 
#define DANGER_DISTANCE_M 20 // Set proximity distance for when neighbouring boats are too close. 

// Library objects
DeviceConfig config;
DeviceRegistry registry;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;
TTS tts;

// Global variables
String name; // Device name. used for print statements.  
uint8_t device_ID; // ID of the device. Configured by setDevice. 
float distanceToTarget; //
float myLat, myLon; // Devices own geographical coordinates. 
uint16_t boatHeading; // Devices own current heading. 
uint16_t buoyHeading; // Bearing from boat to buoy. 
uint16_t expectHeading; //
uint32_t ts;
float danger_dist = DANGER_DISTANCE_M;
uint16_t lastGPSLog = 0;
uint8_t target;
uint8_t start;
uint8_t now;
volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;

/**
 * Interrupt function. 
 * IRAM_ATTR tells linker to insert function into isntruction RAM. makes execution significantly faster. 
 * When PPS pin rises, interrupt function will commence. 
 */
void IRAM_ATTR onPPS() {
  unsigned long now = micros();
  // Debounce: ignore if less than 500ms since last 
  if (now - lastPPSTime > 500000) { // If there is noise with the PPS signal, we don't listen to it if the two instances are too frequent. 
    tdma->onPPSInterrupt(now);
  }
}


/**
 * Setup procedure. 
 * Deals with initialising classes and device components to ensure they are operational. 
 */
void setup() {
  Serial.begin(115200);

  // Load stored device configuration
  if (config.begin()) {
    name = config.getDeviceName();
    device_ID = config.getDeviceID();
  } else {
    Serial.println(name);
    Serial.print(" config failed to start!");
    while(true);
  }
  // Initialize GPS
  if (!gps.begin()) {
    Serial.println(name);
    Serial.print(" GPS failed to start!");
    while (true);
  }
  // Initialize LoRa
  if (!lora.begin()) {
    Serial.println(name);
    Serial.print(" LoRa failed to start!");
    while (true);
  }

  // Initialize navigation system using local (Central, Sydney) reference latitude for precise coordinate data. 
  nav = new Navigation(-33.8688);

  // Define interrupt pins for PPS. 
  pinMode(PPS_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), onPPS, RISING);

  // Initialize TDMA Scheduler (up to 10 devices, 100ms per device).
  tdma = new TDMAScheduler(device_ID, MaxDevice, 100);

  buttonSetup();
  tts.begin();
  Serial.println(name);
  Serial.println(" Setup complete!");

  start = micros();
}



/**
 * Loop. 
 * 
 */
void loop () {
  now = micros();
  // Update GPS data
  gps.update();

  //update position data
  if (gps.getPosition(myLat, myLon)) {
    ts = gps.getGPSTimeMillis();
    // Every second, log your own GPS
    if (millis() - lastGPSLog >= 1000) { 
      registry.addGPSHistory(myLat, myLon, millis());
      // Calculate boat heading based on GPS history
      boatHeading = nav->computeHeadingTrend(registry.getGPSHistoryArray(), registry.getHistoryCount());
      lastGPSLog = millis();
    }
  }

  // transmit slot 
  if (tdma && tdma->canTransmit()) {
    Serial.println("Attempting send protocol...");
    // Create packet and send
    PositionPacket send_packet = Protocol::createPositionPacket(
        DEVICE_TYPE_BOAT, device_ID, myLat, myLon, boatHeading, ts);

    if (lora.transmit(send_packet)) {
        Serial.println(name);
        Serial.print(" packet sent!");
    }
  }

  //receive slot 
  else {
    lora.startReceive();
    if (now - start > 1000000) {
      Serial.println("Listening...");
      start = micros();
    }
  }
  
  // Debug function
  if (lora.receive(packet)) {
    Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
    if (now - start > 2000000) {
      Serial.println("Found packet!"); //these debug statements should be inside the class files. 
      start = micros();
    }
    /*
    Serial.println("---- Position Packet ----");
    Serial.print("Device Type: "); Serial.println(packet.deviceType);
    Serial.print("Device ID: "); Serial.println(packet.deviceID);
    Serial.print("Latitude: "); Serial.println(packet.latitude, 6);
    Serial.print("Longitude: "); Serial.println(packet.longitude, 6);
    Serial.print("Heading: "); Serial.println(packet.heading);
    Serial.print("Timestamp: "); Serial.println(packet.timestamp);
    Serial.print("RSSI: "); Serial.print(lora.getLastRSSI()); Serial.println(" dBm");
    Serial.print("SNR: "); Serial.print(lora.getLastSNR()); Serial.println(" dB");
    Serial.println("--------------------------");
    */
  }

  
  // Calculate euclidean distance when receiving packets
  if (lora.receive(packet) && packet.deviceID != device_ID) {
    if (gps.getPosition(myLat, myLon)) {
      if (packet.deviceType == DEVICE_TYPE_BOAT && packet.deviceID != device_ID) {
        distanceToTarget = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        registry.updateBoat(packet.deviceID, distanceToTarget);
      } else if (packet.deviceType == DEVICE_TYPE_BUOY) {
        distanceToTarget = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        buoyHeading = nav->bearingTo(myLat, myLon, packet.latitude, packet.longitude);
        registry.updateBuoy(packet.deviceID, distanceToTarget, buoyHeading);
      }
    }
  }

  //initiate beeper for boat
  for (int i = 0; i < MaxBoat; i++) {
    float d = registry.getBoatDistance(i);
    if (d > 0 && d <= danger_dist) {
      tts.sayWarning();
    }
  }

  //decide target buoy as the one with smallest id
  //switch target to the closest higher id after passing the target
  float prevd = 100;
  for (int i = 0; i < MaxBuoy; i++) {
    float d = registry.findBuoy(i);
    if (d < prevd) {
      target = registry.getBuoyID(i);
    }
  }
  
  //if button pressed, output reading
  if(speakReading){
    // TODO this is where the tts should give more specific gps information
    dist = registry.getBuoyDistance(target);
    buoyheading = registry.getBuoyHeading(target);
    expectheading = abs(boatheading - buoyheading);
    tts.sayReport(expectheading, dist);
    speakReading = false;
  }

  //shutdown procedure
  uint8_t finish = micros();
  Serial.println(finish - now);
}
