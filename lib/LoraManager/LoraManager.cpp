// lib/LoRaManager/LoRaManager.cpp
#include "LoraManager.h"

LoRaManager::LoRaManager() 
  : radio(nullptr), initialized(false),
    packetsSent(0), packetsReceived(0), 
    packetsFailed(0), checksumErrors(0) {
}

LoRaManager::~LoRaManager() {
  if (radio) {
    delete radio;
  }
}

bool LoRaManager::begin(float freq, float bw, uint8_t sf) {
  radio = new SX1276(new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1));
  
  Serial.print("Initializing LoRa radio... ");
  
  int state = radio->begin(
    freq,    // Frequency
    bw,      // Bandwidth
    sf,      // Spreading factor
    5,       // Coding rate (4/5)
    0x12,    // Sync word (must match all devices)
    20       // Output power (dBm)
  );
  
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("failed, code ");
    Serial.println(state);
    initialized = false;
    return false;
  }
  
  Serial.println("success!");
  
  // Start in receive mode. will be in this 95% of the time. 
  radio->startReceive();
  initialized = true;
  
  return true;
}

bool LoRaManager::transmit(const PositionPacket& packet) {
  if (!initialized) {
    return false;
  }
  
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&packet); //converts packet structure into byte array . radio lib transmit expects byte array. 
  int state = radio->transmit(const_cast<uint8_t*>(bytes), sizeof(PositionPacket)); //sends packet. 
  
  if (state == RADIOLIB_ERR_NONE) {
    packetsSent++;
    
    // Return to receive mode
    radio->startReceive();
    return true;
  } else {
    packetsFailed++;
    Serial.print("Transmit failed, code: ");
    Serial.println(state);
    
    // Try to recover
    radio->startReceive();
    return false;
  }
}

bool LoRaManager::available() {
  if (!initialized) {
    return false;
  }
  
  return radio->getPacketLength() > 0;
}

bool LoRaManager::receive(PositionPacket& packet) {
  if (!initialized || !available()) {
    return false;
  }
  
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&packet); //pass in an empty packet, radio lib will populate it. 
  int state = radio->readData(bytes, sizeof(PositionPacket)); 
  
  if (state == RADIOLIB_ERR_NONE) {
    // Validate packet
    if (Protocol::validatePacket(&packet)) {
      packetsReceived++;
      return true;
    } else {
      checksumErrors++;
      Serial.println("Checksum error!");
      return false;
    }
  }
  
  return false;
}

int16_t LoRaManager::getLastRSSI() {
  if (!initialized) {
    return 0;
  }
  return radio->getRSSI();
}

float LoRaManager::getLastSNR() {
  if (!initialized) {
    return 0.0;
  }
  return radio->getSNR();
}

void LoRaManager::startReceive() {
  if (initialized) {
    radio->startReceive();
  }
}