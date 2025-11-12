/**
 * 
 */

// Include all libraries
#include <Arduino.h>
#include <DeviceConfig.h>
#include <DeviceRegistry.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>

// Definitions for Config variable
#define MaxDevice (DeviceConfig::MaxDeviceNumber)
#define MaxBoat (DeviceConfig::MaxBoatNumber)
#define MaxBuoy (DeviceConfig::MaxBuoyNumber)
#define PPS_PIN 27
#define SIREN_PIN 5

// Library objects
DeviceConfig config;
DeviceRegistry registry;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;

// Constants
const float siren_dist = 30.0f;

// Global Variables
String name;
uint8_t device_ID;
float distanceToTarget;
float myLat, myLon;
uint16_t heading;
uint32_t ts;
uint32_t lastGPSLog = 0;
uint8_t target;
uint8_t start;
uint8_t now;
bool activate_siren = false;

volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;
volatile bool hasTransmittedThisCycle = false;

// Interrupt function
void IRAM_ATTR onPPS() {
  unsigned long now_us = micros();
  // Debounce: ignore if less than 500ms since last 
  if (now_us - lastPPSTime > 500000) { // If there is noise with the PPS signal, we don't listen to it if the two instances are too frequent. 
    tdma->onPPSInterrupt(now_us);
    hasTransmittedThisCycle = false;
  }
}

void setup() {
  //startup procedure
  Serial.begin(115200);

  delay(1000);

  // Load stored configuration
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

  // Initialize Navigation system using arbitrary reference (updated later)
  nav = new Navigation(-33.8688);
  Serial.println("Navigation system initialised");

  // Define interrupt pins for PPS. 
  pinMode(PPS_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), onPPS, RISING);
  Serial.println("PPS interrupt configured");

  // Set Siren pin
  pinMode(SIREN_PIN, OUTPUT);

  // Initialize TDMA Scheduler (up to 10 devices, 100ms per device)
  tdma = new TDMAScheduler(device_ID, MaxDevice, 80, 100);
  Serial.println("TDMA Scheduler Initialsied");

  lastGPSLog = millis() - 1000;

  Serial.println(" Setup complete!");
}

/**
 * Main loop.
 */
void loop () {

  unsigned long now_us = micros();   // For TDMA & precise scheduling
  unsigned long now_ms = millis();   // For logging and human-scale timing

  gps.update();

  //update position data
  if (gps.getPosition(myLat, myLon)) {
    heading = gps.getHeading();
    ts = gps.getGPSTimeMillis();
  }
  // --- GPS Update ---
  gps.update(); 
  if (now_ms - lastGPSLog >= 1000) { 
    if (gps.getPosition(myLat, myLon)) { // Every second, log your own GPS
      ts = gps.getGPSTimeMillis();
      registry.addGPSHistory(myLat, myLon, millis());
      heading = nav->computeHeadingTrend(registry.getGPSHistoryArray(), registry.getHistoryCount()); // Calculate boat heading based on GPS history
      lastGPSLog = now_ms;
    }
  }

  // --- TDMA: Transmit or Receive ---
  if (tdma && tdma->canTransmit()) {
    Serial.println("Attempting send protocol...");
    // Create packet and send
    if (!hasTransmittedThisCycle) {
      PositionPacket send_packet = Protocol::createPositionPacket(
          DEVICE_TYPE_BUOY, device_ID, myLat, myLon, heading, ts);

      if (lora.transmit(send_packet)) {
          Serial.println(name + " packet sent!");
          //packetSent variable true; so two packets aren't set at the same time. 
      hasTransmittedThisCycle = true;
      }
    }
  }
  
  // Calculate euclidean distance when receiving packets
  if (lora.receive(packet) && packet.deviceID != device_ID) {
    Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
    if (gps.getPosition(myLat, myLon)) {
      if (packet.deviceType == DEVICE_TYPE_BOAT) {
        distanceToTarget = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        if (distanceToTarget < siren_dist) {
          activate_siren = true;
        } else {activate_siren = false;}
      }
    }
  }

  //if in range, turn on siren
  //else turn off
  if (activate_siren) {
    // Activate siren
    digitalWrite(SIREN_PIN, LOW);
  }
  else {digitalWrite(SIREN_PIN, HIGH);}
}