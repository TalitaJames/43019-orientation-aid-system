/**
 * @file LoraManager.h
 * @author Milo
 * @brief Lora method functions.  
 * @date 2025-10-20
 */

#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>
#include <RadioLib.h>
#include <Protocol.h>

// LoRa pin definitions (adjust for your board)
#define LORA_CS    13
#define LORA_DIO0  26
#define LORA_RST   14
#define LORA_DIO1  3

class LoRaManager {
private:
  SX1276* radio;
  bool initialized;
  
  // Statistics
  uint32_t packetsSent;
  uint32_t packetsReceived;
  uint32_t packetsFailed;
  uint32_t checksumErrors;
  
public:

  /**
   * Initialises all member variables to safe starting values. 
   */
  LoRaManager();

  /**
   * Cleans up memory when LoraManager is destroyed. 
   */
  ~LoRaManager();
  
  /**
   * @brief Creates the radio object, a reference to the Lora Module.
   */
  bool begin();
  
  /**
   * @brief Transmits package using packet structure. 
   * @param packet defined packet structure, turned into a byte array. 
   * @note See Protocol.h
   */
  bool transmit(const PositionPacket& packet);
  
  /**
   * @brief Checks for available packets. 
   * Looks for packets with a size greater than zero.
   */
  bool available();
  
  /**
   * @brief Receive packet (non-blocking)
   * @param packet Reads data in from radio to the empty packet. 
   */
  bool receive(PositionPacket& packet);
  
  /**
   * @brief Get RSSI of last received packet
   * How strong the last signal was. 
   */
  int16_t getLastRSSI();
  
  /**
   * @brief Get SNR of last received packet
   */
  float getLastSNR();
  
  /**
   * @brief Get statistics, check if packet received successfully
   */
  uint32_t getPacketsSent() const { return packetsSent; }
  uint32_t getPacketsReceived() const { return packetsReceived; }
  uint32_t getPacketsFailed() const { return packetsFailed; }
  uint32_t getChecksumErrors() const { return checksumErrors; }
  
  /**
   * @brief Reset to receive mode
   */
  void startReceive();
};

#endif