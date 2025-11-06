// Include all libraries
#include <Arduino.h>
#include <DeviceConfig.h>
#include <DeviceRegistry.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>

#define MaxDevice (DeviceConfig::MaxDeviceNumber)
#define MaxBoat (DeviceConfig::MaxBoatNumber)
#define MaxBuoy (DeviceConfig::MaxBuoyNumber)
#define PPS_PIN 4
#define SIREN_PIN 300

// Library objects
DeviceConfig config;
DeviceRegistry registry;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;

// Global Variables
String name;
uint8_t ID;
float dist;
float myLat, myLon;
uint16_t heading;
uint32_t ts;
float siren_dist = 30;
uint16_t lastGPSLog = 0;
uint8_t target;
uint8_t start;
uint8_t now;
volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;

// Interrupt function
void IRAM_ATTR onPPS() { //this should be in main?
  unsigned long now = micros();
  //Debounce: ignore if less than 500ms since last 
  if (now - lastPPSTime > 500000) { //if there is noise with the PPS signal, we don't listen to it if the time is too close. 
    tdma->onPPSInterrupt(now);
  }
}

void setup() {
  //startup procedure
  Serial.begin(115200);

  // Load stored configuration
  if (config.begin()) {
    name = config.getDeviceName();
    ID = config.getDeviceID();
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

  pinMode(PPS_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), onPPS, RISING);

  // Initialize TDMA Scheduler (up to 10 devices, 100ms per device)
  tdma = new TDMAScheduler(ID, MaxDevice, 100);

  Serial.println(name);
  Serial.println(" Setup complete!");
  start = micros();
}

void loop () {
  now = micros();
  // Update GPS data
  gps.update();

  //update position data
  if (gps.getPosition(myLat, myLon)) {
    heading = gps.getHeading();
    ts = gps.getGPSTimeMillis();
  }

  // transmit slot 
  if (tdma && tdma->canTransmit()) {
    Serial.println("Attempting send protocol...");
    // Create packet and send
    PositionPacket send_packet = Protocol::createPositionPacket(
        DEVICE_TYPE_BOAT, ID, myLat, myLon, heading, ts);

    if (lora.transmit(send_packet)) {
        Serial.println(name);
        Serial.print(" packet sent!");
    }
  }

  //receive slot 
  else {
    lora.startReceive();
    if (now - start > 2000000) {
      Serial.println("Listening...");
      start = micros();
    }
  }
  
  // Debug function
  if (lora.receive(packet)) {
    Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
    if (now - start > 2000000) {
      Serial.println("Found packet!");
      start = micros();
    }
  }
  
  // Calculate euclidean distance when receiving packets
  if (lora.receive(packet) && packet.deviceID != ID) {
    if (gps.getPosition(myLat, myLon)) {
      if (packet.deviceType == DEVICE_TYPE_BOAT && packet.deviceID != ID) {
        dist = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        registry.updateBoat(packet.deviceID, dist);
      }
    }
  }

  //if in range, turn on siren
  //else turn off
  for (int i = 0; i < MaxBoat; i++) {
    float d = registry.getBoatDistance(i);
    if (d > 0 && d <= siren_dist) {
      // Activate siren
      digitalWrite(SIREN_PIN, 1);
    }
  }
}

