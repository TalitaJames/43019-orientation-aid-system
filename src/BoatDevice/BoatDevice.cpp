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

// Library objects
DeviceConfig config;
DeviceRegistry registry;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;
TTS tts;

// Constants
const float DANGER_DISTANCE_M = 20.0f;
const uint32_t PACKET_DEBOUNCE_MS = 50; // Ignore duplicate packets within 50ms

// Global variables
String name; // Device name. used for print statements.  
uint8_t device_ID; // ID of the device. Configured by setDevice. 
float distanceToTarget; //
float myLat, myLon; // Devices own geographical coordinates. 
uint16_t boatHeading; // Devices own current heading. 
uint16_t buoyHeading; // Bearing from boat to buoy. 
uint16_t expectHeading; // 
uint32_t ts;
uint32_t lastGPSLog = 0;
uint8_t target;

volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;
volatile bool hasTransmittedThisCycle = false;

/**
 * Interrupt function. 
 * IRAM_ATTR tells linker to insert function into isntruction RAM. makes execution significantly faster. 
 * When PPS pin rises, interrupt function will commence. 
 */
void IRAM_ATTR onPPS() {
  unsigned long now_us = micros();
  // Debounce: ignore if less than 500ms since last 
  if (now_us - lastPPSTime > 500000) { // If there is noise with the PPS signal, we don't listen to it if the two instances are too frequent. 
    tdma->onPPSInterrupt(now_us);
    hasTransmittedThisCycle = false;
  }
}


/**
 * Setup procedure. 
 * Deals with initialising classes and device components to ensure they are operational. 
 */
void setup() {
  Serial.begin(115200);
  
  delay(1000);

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
  Serial.println("Navigation system initialised");

  // Define interrupt pins for PPS. 
  pinMode(PPS_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), onPPS, RISING);
  Serial.println("PPS interrupt configured");

  // Initialize TDMA Scheduler (up to 10 devices, 100ms per device).
  tdma = new TDMAScheduler(device_ID, MaxDevice, 80, 100);
  Serial.println("TDMA Scheduler Initialsied");

  // Initialise button 
  buttonSetup();
  tts.begin();
  Serial.println("Button initialised");

  lastGPSLog = millis() - 1000;

  Serial.println(" Setup complete!");
}


/**
 * Main loop. 
 * 
 */
void loop () {
  unsigned long now_us = micros();   // For TDMA & precise scheduling
  unsigned long now_ms = millis();   // For logging and human-scale timing

  // --- GPS Update ---
  gps.update(); 
  if (now_ms - lastGPSLog >= 1000) { 
    if (gps.getPosition(myLat, myLon)) { // Every second, log your own GPS
      ts = gps.getGPSTimeMillis();
      registry.addGPSHistory(myLat, myLon, millis());
      boatHeading = nav->computeHeadingTrend(registry.getGPSHistoryArray(), registry.getHistoryCount()); // Calculate boat heading based on GPS history
      lastGPSLog = now_ms;
    }
  }

  // --- TDMA: Transmit or Receive ---
  if (tdma && tdma->canTransmit()) {
    Serial.println("Attempting send protocol...");
    // Create packet and send
    if (!hasTransmittedThisCycle) {
      PositionPacket send_packet = Protocol::createPositionPacket(
          DEVICE_TYPE_BOAT, device_ID, myLat, myLon, boatHeading, ts);

      if (lora.transmit(send_packet)) {
          Serial.println(name + " packet sent!");
          //packetSent variable true; so two packets aren't set at the same time. 
      hasTransmittedThisCycle = true;
      }
    }
  }

  // else {
  //   // Radio is already in receive mode from transmit() or previous loop
  //   // Just check for incoming packets (handled later in your code)

  //   // lora.startReceive();
  //   // if (now - start > 1000000) {
  //   //   Serial.println("Listening...");
  //   //   start = micros();
  //   // }

  // }
  
  // // Debug function
  // if (lora.receive(packet)) {
  //   Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
  //   if (now - start > 2000000) {
  //     Serial.println("Found packet!"); //these debug statements should be inside the class files. 
  //     start = micros();
  //   }
  //   /*
  //   Serial.println("---- Position Packet ----");
  //   Serial.print("Device Type: "); Serial.println(packet.deviceType);
  //   Serial.print("Device ID: "); Serial.println(packet.deviceID);
  //   Serial.print("Latitude: "); Serial.println(packet.latitude, 6);
  //   Serial.print("Longitude: "); Serial.println(packet.longitude, 6);
  //   Serial.print("Heading: "); Serial.println(packet.heading);
  //   Serial.print("Timestamp: "); Serial.println(packet.timestamp);
  //   Serial.print("RSSI: "); Serial.print(lora.getLastRSSI()); Serial.println(" dBm");
  //   Serial.print("SNR: "); Serial.print(lora.getLastSNR()); Serial.println(" dB");
  //   Serial.println("--------------------------");
  //   */
  // }

  
  // Calculate euclidean distance when receiving packets
  if (lora.receive(packet) && packet.deviceID != device_ID) { 
    Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
    if (gps.getPosition(myLat, myLon)) {
      if (packet.deviceType == DEVICE_TYPE_BOAT) {
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
    if (d > 0 && d <= DANGER_DISTANCE_M) {
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
    //distanceToTarget = registry.getBuoyDistance(target);
    buoyHeading = registry.getBuoyHeading(target);
    distanceToTarget = registry.getBoatDistance(1);
    expectHeading = abs(boatHeading - buoyHeading);
    tts.sayReport(expectHeading, distanceToTarget);
    speakReading = false;
  }

  //shutdown procedure
  unsigned long finish_ms = millis();
  //Serial.print("Loop time: ");
  //Serial.print(finish_ms - now_ms);
  //Serial.println(" ms");
}



//flag for sending and receiving. we only need to read the packet one time, and then also send the packet one time. 