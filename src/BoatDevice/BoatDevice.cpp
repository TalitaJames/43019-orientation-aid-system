
#include <Arduino.h>
#include <DeviceConfig.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>

#define MaxDevice (DeviceConfig::MaxDeviceNumber)
#define MaxBoat (DeviceConfig::MaxBoatNumber)
#define MaxBuoy (DeviceConfig::MaxBuoyNumber)
#define PPS_PIN 4

struct DeviceInfo {
  uint8_t id;
  float distance;
};
DeviceInfo devices[MaxDevice];
int deviceCount = 0;

DeviceConfig config;
GPSManager gps;
LoRaManager lora;
PositionPacket packet;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;
String name;
uint8_t ID;
float dist;
float myLat, myLon;
uint16_t heading;
uint32_t ts;
float danger_dist = 20;

//GLOBAL VARIABLES
volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;

//
void IRAM_ATTR onPPS() { //this should be in main?
  unsigned long now = micros();
  //Debounce: ignore if less than 500ms since last 
  if (now - lastPPSTime > 500000) { //if there is noise with the PPS signal, we don't listen to it if the time is too close. 
    tdma->onPPSInterrupt(now);
  }
}

void updateDevice(uint8_t id, float distance) {
  // Check if device already exists
  for (int i = 0; i < deviceCount; i++) {
    if (devices[i].id == id) {
      devices[i].distance = distance;
      return;
    }
  }
  // Add new device if space available
  if (deviceCount < MaxDevice) {
    devices[deviceCount].id = id;
    devices[deviceCount].distance = distance;
    deviceCount++;
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
}

void loop () {
  // Update GPS data
  gps.update();

  //update position data
  if (gps.getPosition(myLat, myLon)) {
    heading = gps.getHeading();
    ts = gps.getGPSTimeMillis();
  }

  // transmit slot 
  if (tdma && tdma->canTransmit()) {
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
    if(lora.receive(packet)) {
      Serial.printf("Received packet from %d at %lu us\n", packet.deviceID, micros() - lastPPSTime);
      
    }
    lora.startReceive();
    Serial.println("Listening...");
  }
  
  // Debug function
  if (lora.receive(packet)) {
    Serial.println("Found packet!");
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
      dist = nav->distanceBetween(myLat, myLon, packet.latitude, packet.longitude);
      updateDevice(packet.deviceID, dist);
    }
  }

  //initiate beeper for boat
  for (int i = 0; i < deviceCount; i++) {
    if (devices[i].id <= MaxBoat) { // only boats trigger beeper
      if (devices[i].distance <= danger_dist) {
        //turn on warning beep or sth
      }
    }
  }

  // ...
  float lat, lon;
  if (gps.getPosition(lat, lon)) {
    gps.recordPosition(lat, lon);

    const GPSDataPoint* points = gps.getHistory();
    uint8_t count = gps.getHistoryCount();
  }

  //decide target buoy as the one with smallest id
  //switch target to the closest higher id after passing the target

  //prepare 'sentence' to output for spoken readings. 

  //if button pressed, output reading

  //shutdown procedure
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


#include <Arduino.h>
#include <RadioLib.h>

// --- Hardware Config ---
#define LORA_CS    13
#define LORA_DIO0  26
#define LORA_RST   14
#define LORA_DIO1  3
#define PPS_PIN    27   // Example PPS pin

// --- Radio Setup ---
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1);

// --- Global Variables ---
volatile unsigned long lastPPSTime = 0;
volatile bool ppsTriggered = false;

void IRAM_ATTR onPPS() {
  unsigned long now = micros();
  // Debounce: ignore if less than 500 ms since last
  if (now - lastPPSTime > 500000) {
    lastPPSTime = now;
    ppsTriggered = true;
  }
}

// --- PPSDeviceScheduler Class ---
class PPSDeviceScheduler {
public:
  int deviceID;
  unsigned long slotDuration;

  PPSDeviceScheduler(int id, unsigned long slotMs = 100)
    : deviceID(id), slotDuration(slotMs * 1000UL) {}

  void begin() {
    pinMode(PPS_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PPS_PIN), onPPS, RISING);

    Serial.println("=== LoRa Device Scheduler ===");
    Serial.print("Device ID: "); Serial.println(deviceID);
  }

  void loop() {
    if (ppsTriggered) {
      ppsTriggered = false;
      unsigned long baseTime = lastPPSTime;

      unsigned long slotStart = baseTime + (deviceID - 1) * slotDuration;
      unsigned long slotEnd   = slotStart + slotDuration;

      while (micros() < slotStart) {
        // wait until our time slot
      }
      
      executeSlot();
    }
  }

  void executeSlot() {
  unsigned long tStart = micros();
  String message = "Hello from device " + String(deviceID);

  // Convert to char buffer if needed by your LoRa function
  int state = radio.transmit(message);
  unsigned long tEnd = micros();

  Serial.println("---- Slot Diagnostics ----");
  Serial.print("Started at: "); 
  Serial.print(tStart - lastPPSTime); 
  Serial.println(" us after PPS");

  Serial.print("Ended at:   "); 
  Serial.print(tEnd - lastPPSTime); 
  Serial.println(" us after PPS");

  Serial.print("Transmission duration: "); 
  Serial.print(tEnd - tStart); 
  Serial.println(" us");

  if (state == RADIOLIB_ERR_NONE)
    Serial.println("✓ Transmission OK");
  else {
    Serial.print("✗ Transmission failed, code: ");
    Serial.println(state);
  }

  Serial.println("--------------------------");
}


};

// --- Instantiate the Device ---
PPSDeviceScheduler scheduler(5);  // Example: device 3 out of 10

void setup() {
  Serial.begin(115200);
  delay(1500);

  int state = radio.begin(915.0, 125.0, 7, 7, 0x12, 10, 8, 0);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  scheduler.begin();
}

void loop() {
  scheduler.loop();
}




// onPPS() interrupt → tells TDMA that a new cycle started
// loop():
//     tdma.update()
//     if (tdma.canTransmit())
//          doTransmit()
//     else
//          doReceive()
