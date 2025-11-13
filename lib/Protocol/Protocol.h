/**
 * @file Protocol.h
 * @author Milo
 * @brief Defining the packet structures that are transmitted. 
 * @date 2025-10-13
 * 
 * Defines the packet structure 
 * calculates checksums
 * validates received packets
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
  uint8_t deviceID;      // Max number defined in DeviceConfig.h
  float latitude;        // 4 bytes
  float longitude;       // 4 bytes
  uint16_t heading;      // 0-359 degrees (from IMU/GPS)
  uint32_t timestamp;    // GPS time in milliseconds since midnight
  uint8_t checksum;      // CRC8 checksum
} __attribute__((packed)); //dont add invisible bytes 

class Protocol {
public:
  /**
   * @brief Calculate an 8-bit CRC checksum for given data.
   * @param data Pointer to data buffer.
   * @param length Number of bytes to process.
   * @return Computed CRC8 checksum.
   */
  static uint8_t calculateChecksum(const uint8_t* data, size_t length);
  
  /**
   * @brief Validate the integrity of a received position packet.
   * @param packet Pointer to the received packet.
   * @return True if checksum and structure are valid.
   */
  static bool validatePacket(const PositionPacket* packet);
  
  /**
   * @brief Construct a new position packet with provided data.
   * @param deviceType Type of the sending device.
   * @param deviceID Device identifier.
   * @param lat Latitude in decimal degrees.
   * @param lon Longitude in decimal degrees.
   * @param heading Heading in degrees (0–359).
   * @param timestamp GPS time in milliseconds since midnight.
   * @return Fully assembled PositionPacket.
   */
  static PositionPacket createPositionPacket(
    DeviceType deviceType,
    uint8_t deviceID,
    float lat,
    float lon,
    uint16_t heading,
    uint32_t timestamp
  );
  
  /**
   * @brief Get the size of a position packet in bytes.
   * @return Packet size (25 bytes).
   */
  static constexpr size_t getPacketSize() { return sizeof(PositionPacket); }
};

#endif