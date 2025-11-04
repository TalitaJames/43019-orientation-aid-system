// lib/Protocol/Protocol.cpp
#include "Protocol.h"

uint8_t Protocol::calculateChecksum(const uint8_t* data, size_t length) {
  uint8_t crc = 0x00;
  
  // CRC8 calculation (exclude last byte which is checksum itself)
  for (size_t i = 0; i < length - 1; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;  // CRC8 polynomial
      } else {
        crc <<= 1;
      }
    }
  }
  
  return crc;
}

bool Protocol::validatePacket(const PositionPacket* packet) {
  // Check header
  if (packet->header != 0xAA) {
    return false;
  }
  
  // Check packet type
  if (packet->packetType != PACKET_TYPE_POSITION) {
    return false;
  }
  
  // Verify checksum
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(packet);
  uint8_t calculatedChecksum = calculateChecksum(bytes, sizeof(PositionPacket));
  
  return (packet->checksum == calculatedChecksum);
}

PositionPacket Protocol::createPositionPacket(
  DeviceType deviceType,
  uint8_t deviceID,
  float lat,
  float lon,
  uint16_t heading,
  uint32_t timestamp
) {
  PositionPacket packet;
  packet.header = 0xAA;
  packet.packetType = PACKET_TYPE_POSITION;
  packet.deviceType = deviceType;
  packet.deviceID = deviceID;
  packet.latitude = lat;
  packet.longitude = lon;
  packet.heading = heading;
  packet.timestamp = timestamp;
  
  // Calculate and set checksum
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&packet);
  packet.checksum = calculateChecksum(bytes, sizeof(PositionPacket));
  
  return packet;
}