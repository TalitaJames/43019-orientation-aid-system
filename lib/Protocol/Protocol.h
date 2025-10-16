/**
 * @file Protocol.h
 * @author Milo
 * @brief Defining the packet structures that are transmitted. 
 * @date 2025-10-13
 * 
 * Defines the packet structure 
 * calculates checksums
 * validates received packets
 *
 */


#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

// Device types
enum DeviceType : uint8_t {
  DEVICE_TYPE_BOAT = 1,
  DEVICE_TYPE_BUOY = 2
};

// Packet types
enum PacketType : uint8_t {
  PACKET_TYPE_POSITION = 0x01,
  PACKET_TYPE_HEARTBEAT = 0x02
};

// Position packet (25 bytes total)
struct PositionPacket {
  uint8_t header;        // 0xAA - packet start marker
  uint8_t packetType;    // PACKET_TYPE_POSITION
  uint8_t deviceType;    // DEVICE_TYPE_BOAT or DEVICE_TYPE_BUOY
  uint8_t deviceID;      // 1-20 for boats, 21-22 for buoys
  float latitude;        // 4 bytes
  float longitude;       // 4 bytes
  uint16_t heading;      // 0-359 degrees (from IMU/GPS)
  uint16_t speed;        // Speed in cm/s (0-65535) can GPS give speed? is this relevant?
  uint32_t timestamp;    // GPS time in milliseconds since midnight
  uint8_t checksum;      // CRC8 checksum
} __attribute__((packed)); //dont add invisible bytes 

class Protocol {
public:
  // Calculate CRC8 checksum
  static uint8_t calculateChecksum(const uint8_t* data, size_t length);
  
  // Validate packet integrity
  static bool validatePacket(const PositionPacket* packet);
  
  // Create position packet
  static PositionPacket createPositionPacket(
    DeviceType deviceType,
    uint8_t deviceID,
    float lat,
    float lon,
    uint16_t heading,
    uint16_t speed,
    uint32_t timestamp
  );
  
  // Get packet size
  static constexpr size_t getPacketSize() { return sizeof(PositionPacket); }
};

#endif