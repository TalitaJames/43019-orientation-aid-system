
#include <Arduino.h>
#include <DeviceConfig.h>
#include <DeviceRegistry.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>
#include <TTS.h>

#define MaxDevice (DeviceConfig::MaxDeviceNumber)
#define MaxBoat (DeviceConfig::MaxBoatNumber)
#define MaxBuoy (DeviceConfig::MaxBuoyNumber)

DeviceConfig config;
DeviceRegistry registry;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;
TTS tts;
String name;
uint8_t ID;
float dist;
float myLat, myLon;
uint16_t boatheading;
uint16_t buoyheading;
uint16_t expectheading;
uint32_t ts;
float danger_dist = 20;
uint16_t lastGPSLog = 0;
uint8_t target;
uint8_t start;
uint8_t now;

void setup() {
  //startup procedure
  Serial.begin(115200);
  tts.begin();

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

  // Initialize TDMA Scheduler (assume up to 5 boats, 1s per slot)
  tdma = new TDMAScheduler(ID, MaxDevice, 100);

  Serial.println(name);
  Serial.println(" Setup complete!");
  start = micros();
}

void loop () {
  now = micros();
  // Update GPS data
  gps.update();
  if (gps.getPosition(myLat, myLon)) {
    ts = gps.getGPSTimeMillis();
    // Every second, log your own GPS
    if (millis() - lastGPSLog >= 1000) {
      registry.addGPSHistory(myLat, myLon, millis());
      // Calculate boat heading based on GPS history
      boatheading = nav->computeHeadingTrend(registry.getGPSHistoryArray(), registry.getHistoryCount());
      lastGPSLog = millis();
    }
  }

  // Send when its their turn
  if (tdma && tdma->canTransmit()) {
    Serial.println("Attempting send protocol...");
    // Create packet and send
    PositionPacket send_packet = Protocol::createPositionPacket(
        DEVICE_TYPE_BOAT, ID, myLat, myLon, boatheading, ts
    );

    if (lora.transmit(send_packet)) {
        Serial.println(name);
        Serial.print(" packet sent!");
    }
  }
  else {
    lora.startReceive();
    if (now - start > 2000000) {
      Serial.println("Listening...");
      start = micros();
    }
  }
  
  // Debug function
  if (lora.receive(packet)) {
      if (now - start > 2000000) {
        Serial.println("Found packet!");
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
  if (lora.receive(packet) && packet.deviceID != ID) {
    if (gps.getPosition(myLat, myLon)) {
      if (packet.deviceType == DEVICE_TYPE_BOAT && packet.deviceID != ID) {
        dist = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        registry.updateBoat(packet.deviceID, dist);
      } else if (packet.deviceType == DEVICE_TYPE_BUOY) {
        dist = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
        buoyheading = nav->bearingTo(myLat, myLon, packet.latitude, packet.longitude);
        registry.updateBuoy(packet.deviceID, dist, buoyheading);
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
  if (1 == 2){
    dist = registry.getBuoyDistance(target);
    buoyheading = registry.getBuoyHeading(target);
    expectheading = abs(boatheading - buoyheading);
    tts.sayReport(expectheading, dist);
  }

  //shutdown procedure
  uint8_t finish = micros();
  Serial.println(finish - now);
}

/* 
//Test loop for send and receive LORA without gps
void loop(){
  if (tdma && tdma->canTransmit()) {
    float lat, lon;
    lat = 1;
    lon = 2;
    uint16_t heading = 100;
    uint32_t ts = 1000;

    // Create packet and send
    PositionPacket packet = Protocol::createPositionPacket(
        DEVICE_TYPE_BUOY, ID, lat, lon, heading, ts
    );

    if (lora.transmit(packet)) {
        Serial.println(name);
        Serial.print(" packet sent!");
    }
  } 
  else {
    PositionPacket packet;
    lora.startReceive();
    Serial.println("Listening...");
    if (lora.receive(packet)) {
      Serial.println("Found packet!");
    }
  }
  delay(200);
}
*/
///////////////////////////////////////////////////////////////////
/*
#include <Arduino.h>
#include <RadioLib.h>
 
// Pin definitions - adjust these to match your hardware

#define LORA_CS    13
#define LORA_DIO0  26
#define LORA_RST   14
#define LORA_DIO1  3
 
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1);
 
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== LoRa Test - Device 2 (Receiver) ===");
  Serial.print("Initializing LoRa... ");

  // Initialize with EXACT same parameters as Device 1
  int state = radio.begin(
    915.0,   // Frequency (MHz)
    125.0,   // Bandwidth (kHz)
    7,       // Spreading factor
    7,       // Coding rate (4/7)
    0x12,    // Sync word
    10,      // Output power (dBm)
    8,       // Preamble length
    0        // Gain (0 = auto)
  );

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("FAILED! Error code: ");
    Serial.println(state);
    while (true);
  }

  Serial.println("SUCCESS!");
  Serial.println("Listening for messages...\n");

  // Start receiving
  radio.startReceive();
}
 
void loop() {
  // Check if something was received
  if (radio.getPacketLength() > 0) {
    String message;
    int state = radio.readData(message);
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(message);
    } else {
      Serial.print("✗ Read failed, code: ");
      Serial.println(state);
    }

    // Go back to receive mode
    radio.startReceive();
  }
  delay(100);  // Small delay to prevent CPU hogging
}

////////////////////////////////////////////////////////////////////////
 
void loop() {
  String message = "Hello World 1";
  
  Serial.print("Sending: ");
  Serial.println(message);
  
  int state = radio.transmit(message);
  
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("✓ Transmitted successfully!");
  } else {
    Serial.print("✗ Transmission failed, code: ");
    Serial.println(state);
  }
  
  Serial.println("---");
  delay(2000);  // Send every 2 seconds
}
*/