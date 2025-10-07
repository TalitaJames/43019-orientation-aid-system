#include "LoRaModule.h"
#include <math.h>

LoRaModule::LoRaModule(TinyGPSPlus& gps, long loraFreq)
    : _gps(gps), _loraFreq(loraFreq), nodeCount(0)
{}

void LoRaModule::begin() {
    pinMode(MODE_PIN, INPUT_PULLUP);
    _isBoat = digitalRead(MODE_PIN);  // HIGH = boat, LOW = buoy
    _id = (uint32_t)ESP.getEfuseMac();

    Serial.begin(115200);
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(_loraFreq)) {
        Serial.println("LoRa init failed!");
        while (1);
    }
    Serial.printf("Node started: ID=%lu Type=%s\n", _id, _isBoat ? "boat" : "buoy");
}

void LoRaModule::update() {
    // Update own GPS
    if (_gps.location.isValid()) {
        _lat = _gps.location.lat();
        _lng = _gps.location.lng();
    }

    sendStatus();
    receivePackets();
    reactToNearbyNodes();
}

void LoRaModule::sendStatus() {
    LoRa.beginPacket();
    LoRa.printf("id=%lu,type=%s,lat=%.6f,lng=%.6f", _id, _isBoat ? "boat" : "buoy", _lat, _lng);
    LoRa.endPacket();
}

void LoRaModule::receivePackets() {
    int packetSize = LoRa.parsePacket();
    if (!packetSize) return;

    String packet = "";
    while (LoRa.available()) packet += (char)LoRa.read();

    int idIdx = packet.indexOf("id=");
    int typeIdx = packet.indexOf("type=");
    int latIdx = packet.indexOf("lat=");
    int lngIdx = packet.indexOf("lng=");

    if (idIdx == -1 || typeIdx == -1 || latIdx == -1 || lngIdx == -1) return;

    uint32_t rid = (uint32_t)packet.substring(idIdx + 3, packet.indexOf(",", idIdx)).toInt();
    if (rid == _id) return; // ignore self

    String type = packet.substring(typeIdx + 5, packet.indexOf(",", typeIdx));
    double lat = packet.substring(latIdx + 4, packet.indexOf(",", latIdx)).toDouble();
    double lng = packet.substring(lngIdx + 4).toDouble();

    updateNode(rid, type, lat, lng);
}

void LoRaModule::updateNode(uint32_t id, String type, double lat, double lng) {
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].id == id) {
            nodes[i].lat = lat;
            nodes[i].lng = lng;
            nodes[i].lastSeen = millis();
            return;
        }
    }
    if (nodeCount < 10) {
        nodes[nodeCount++] = { id, type, lat, lng, millis() };
    }
}

double LoRaModule::calculateDistance(double lat1, double lng1, double lat2, double lng2) {
    const double R = 6371000; // meters
    double dLat = radians(lat2 - lat1);
    double dLng = radians(lng2 - lng1);
    double a = sin(dLat/2)*sin(dLat/2) +
               cos(radians(lat1))*cos(radians(lat2))*
               sin(dLng/2)*sin(dLng/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

void LoRaModule::reactToNearbyNodes() {
    for (int i = 0; i < nodeCount; i++) {
        double d = calculateDistance(_lat, _lng, nodes[i].lat, nodes[i].lng);
        if (_isBoat && nodes[i].type == "boat" && d < 50) {
            triggerSpeaker("boat_close");
        }
        else if (!_isBoat && nodes[i].type == "boat" && d < 100) {
            triggerSpeaker("boat_near_buoy");
        }
    }
}

void LoRaModule::triggerSpeaker(String mode) {
    Serial.println("Speaker Trigger: " + mode);
}