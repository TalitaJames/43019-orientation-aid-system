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
    name = config.getDeviceName();
    ID = config.getDeviceID();

    // Load stored configuration
    if (!config.begin()) {
        Serial.println(name);
        Serial.println(" config failed to start!");
        while(true);
    }
    // Initialize GPS
    if (!gps.begin()) {
        Serial.println(name);
        Serial.println(" GPS failed to start!");
        while (true);
    }
    // Initialize LoRa (frequency, bandwidth, SF)
    if (!lora.begin(915.0, 125.0, 7)) {
        Serial.println(name);
        Serial.println(" LoRa failed to start!");
        while (true);
    }

    // Initialize Navigation system using arbitrary reference (updated later)
    nav = new Navigation(0);

    // Initialize TDMA Scheduler (assume up to 5 boats, 1s per slot)
    tdma = new TDMAScheduler(ID, 5, 1000);

    Serial.println(name);
    Serial.println(" Setup complete. Waiting for GPS fix...");
}

void loop () {
    //receive GPS data, send when its their turn
    gps.update();

    if (tdma && tdma->canTransmit()) {
        float lat, lon;
        if (gps.getPosition(lat, lon)) {
            uint16_t heading = gps.getHeading();
            uint16_t speed = gps.getSpeed();
            uint32_t ts = gps.getGPSTimeMillis();

            // Create packet and send
            PositionPacket packet = Protocol::createPositionPacket(
                DEVICE_TYPE_BOAT, ID, lat, lon, heading, speed, ts
            );

            if (lora.transmit(packet)) {
                Serial.println(name);
                Serial.print(" packet sent!");
            }
        }
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