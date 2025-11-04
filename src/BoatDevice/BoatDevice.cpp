
#include <Arduino.h>
#include <DeviceConfig.h>
#include <GPSManager.h>
#include <LoraManager.h>
#include <Navigation.h>
#include <Protocol.h>
#include <TDMAScheduler.h>

DeviceConfig config;
GPSManager gps;
LoRaManager lora;
Navigation* nav = nullptr;
TDMAScheduler* tdma = nullptr;
String name;
uint8_t ID;
float dist;

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

    // Initialize TDMA Scheduler (assume up to 5 boats, 1s per slot)
    tdma = new TDMAScheduler(ID, 5, 1000);

    Serial.println(name);
    Serial.println(" Setup complete!");
}

void loop () {
    //receive GPS data, send when its their turn
    gps.update();

    if (tdma && tdma->canTransmit()) {
        float lat, lon;
        if (gps.getPosition(lat, lon)) {
            uint16_t heading = gps.getHeading();
            uint32_t ts = gps.getGPSTimeMillis();

            // Create packet and send
            PositionPacket packet = Protocol::createPositionPacket(
                DEVICE_TYPE_BOAT, ID, lat, lon, heading, ts
            );

            if (lora.transmit(packet)) {
                Serial.println(name);
                Serial.print(" packet sent!");
            }
        }
    } else if (lora.available()) {
    }

    
    //calculate euclidean distance for nearby boats
    PositionPacket incoming;
    if (lora.receive(incoming)) {
        if (incoming.deviceType == DEVICE_TYPE_BOAT && incoming.deviceID != ID) {
            float myLat, myLon;
            if (gps.getPosition(myLat, myLon)) {
                dist = nav->distanceBetween(myLat, myLon, incoming.latitude, incoming.longitude);
                Serial.print("[Boat] Distance to Boat #");
                Serial.print(incoming.deviceID);
                Serial.print(": ");
                Serial.print(dist);
                Serial.println(" m");
            }
        }
    }

    //initiate beeper for boat

    
    //collect buoy data, and calculate euclidean distance and compare to IMU readings.
    if (lora.receive(incoming)) {
        if (incoming.deviceType == DEVICE_TYPE_BUOY) {
            float myLat, myLon;
            if (gps.getPosition(myLat, myLon)) {
                float dist = nav->distanceBetween(myLat, myLon, incoming.latitude, incoming.longitude);
                float bearing = nav->bearingTo(myLat, myLon, incoming.latitude, incoming.longitude);
                float rel = nav->relativeBearing(bearing, gps.getHeading());

                Serial.print("[Boat] Buoy #");
                Serial.print(incoming.deviceID);
                Serial.print(": ");
                Serial.print(dist);
                Serial.print(" m @ ");
                Serial.print(rel);
                Serial.println("° relative");
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

    //prepare 'sentence' to output for spoken readings. 

    //if button pressed, output reading

    //if boat leaves target buoy range and passes global minima, switch to next target buoy. 

    //shutdown procedure
    
}
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
    10,      // Output power (dBm) - CHANGED FROM 20 to 10
    8,       // Preamble length
    0        // Gain (0 = auto)
  );

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("FAILED! Error code: ");
    Serial.println(state);
    Serial.println("\nError codes:");
    Serial.println("-2 = Invalid frequency");
    Serial.println("-3 = Invalid bandwidth");
    Serial.println("-4 = Invalid spreading factor");
    Serial.println("-5 = Invalid coding rate");
    Serial.println("-6 = Invalid output power");
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
      Serial.println("=== Message Received ===");
      Serial.print("Data: ");
      Serial.println(message);
      Serial.print("RSSI: ");
      Serial.print(radio.getRSSI());
      Serial.println(" dBm");
      Serial.print("SNR: ");
      Serial.print(radio.getSNR());
      Serial.println(" dB");
      Serial.println("========================\n");
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
  
  Serial.println("=== LoRa Test - Device 1 (Transmitter) ===");
  Serial.print("Initializing LoRa... ");
  
  // Initialize with corrected parameters
  int state = radio.begin(
    915.0,   // Frequency (MHz)
    125.0,   // Bandwidth (kHz)
    7,       // Spreading factor
    7,       // Coding rate (4/7)
    0x12,    // Sync word
    10,      // Output power (dBm) - CHANGED FROM 20 to 10
    8,       // Preamble length
    0        // Gain (0 = auto)
  );
  
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("FAILED! Error code: ");
    Serial.println(state);
    Serial.println("\nError codes:");
    Serial.println("-2 = Invalid frequency");
    Serial.println("-3 = Invalid bandwidth");
    Serial.println("-4 = Invalid spreading factor");
    Serial.println("-5 = Invalid coding rate");
    Serial.println("-6 = Invalid output power");
    while (true);
  }
  
  Serial.println("SUCCESS!");
  Serial.println("Starting transmission loop...\n");
}
 
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