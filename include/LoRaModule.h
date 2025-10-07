#ifndef LORAMODULE_H
#define LORAMODULE_H

#include <LoRa.h>
#include <SPI.h>
#include <TinyGPSPlus.h>

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define SS         5
#define RST        26
#define DIO0       27

#define MODE_PIN   4   // determines boat/buoy type

struct NodeInfo {
    uint32_t id;
    String type;
    double lat;
    double lng;
    unsigned long lastSeen;
};

class LoRaModule {
public:
    LoRaModule(TinyGPSPlus& gps, long loraFreq = 915E6);

    void begin();
    void update(); // does both send+receive+react

private:
    void sendStatus();
    void receivePackets();
    void updateNode(uint32_t id, String type, double lat, double lng);
    double calculateDistance(double lat1, double lng1, double lat2, double lng2);
    void reactToNearbyNodes();

    void triggerSpeaker(String mode); // placeholder

    TinyGPSPlus& _gps;
    long _loraFreq;
    bool _isBoat;
    uint32_t _id;

    NodeInfo nodes[10];
    int nodeCount;

    double _lat, _lng;
};

#endif