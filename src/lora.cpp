#include "Lora.h"
#include "main.cpp"

Lora::Lora(TinyGPSPlus& gps, long loraFreq)
    : _gps(gps), _loraFreq(loraFreq)
{}

void Lora::begin() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    Serial.println("LoRa GPS Node Starting...");

    // LoRa setup
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

    if (!LoRa.begin(_loraFreq)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    Serial.println("LoRa init OK");
}

void Lora::update() {
    readGPS();
    sendLocation();
    checkReceived();
}

void Lora::readGPS() {
    while (Serial2.available() > 0) {
        _gps.encode(Serial2.read());
    }
}

void Lora::sendLocation() {
    if (_gps.location.isUpdated()) {
        double lat = _gps.location.lat();
        double lng = _gps.location.lng();

        Serial.print("Sending location: ");
        Serial.print(lat, 6);
        Serial.print(", ");
        Serial.println(lng, 6);

        LoRa.beginPacket();
        LoRa.print("LAT:");
        LoRa.print(lat, 6);
        LoRa.print(",LON:");
        LoRa.print(lng, 6);
        LoRa.endPacket();
    }
}

void Lora::checkReceived() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received packet: ");
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }
        Serial.println();
    }
}