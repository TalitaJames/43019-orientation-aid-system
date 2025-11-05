/**
 * @file TDMAScheduler.h
 * @author Milo
 * @brief Dictates when each device can talk. 
 * @date 2025-10-14
 * 
 */


#ifndef TDMA_SCHEDULER_H
#define TDMA_SCHEDULER_H

#include <Arduino.h>
#include <DeviceConfig.h>

/**
 * @brief Class that defines the timing schedule for the device communication.
 * 
 * We will rely on the GPS time for synchronization as each device will operate and turn on independently. Each ESP32 will have its own GPS module. 
 * The GPS modules can output an NMEA sentence and a PPS pin indicating the time of whole second, accuracy up to nanoseconds. We will use this to synchronise all of the devices. 
 * https://wiki.keyestudio.com/KS0319_keyestudio_GPS_Module
 */
class TDMAScheduler {
private:
  uint8_t myDeviceID;
  uint16_t slotDuration;        // In microseconds (so 100000 for 100ms)
  uint8_t totalSlots;           // Max number defined in DeviceConfig.h
  uint16_t transmitWindow;      // shorter than slot, e.g. 80ms
  uint16_t cycleDuration;       // 1320ms
  
  // GPS time tracking
  uint32_t lastGPSSecond;       // GPS time in whole seconds
  uint32_t lastPPSMicros;       // micros() when PPS pulse arrived
  bool synchronized;
  
  // Calculate current time with microsecond precision
  uint32_t getCurrentTimeMillis() const;
  uint32_t getCurrentTimeMicros() const;
  
  uint32_t getCycleStartTime() const;
  uint32_t getMySlotStartTime() const;
  
public:
  TDMAScheduler(uint8_t deviceID, uint8_t numDevices = DeviceConfig::MaxDeviceNumber, uint16_t slotMs = 100);
  
  // Update with PPS pulse and GPS second
  void updateWithPPS(uint32_t ppsMicros, uint32_t gpsSecond);
  
  // Time elapsed since last PPS (in microseconds)
  uint32_t microsSincePPS() const; //this could replace the other get time functions

  // Check if we should transmit now
  bool canTransmit() const;
  
  // Check if synchronized
  bool isSynchronized() const { return synchronized; }
  
  // Get timing info
  int32_t getTimeUntilMySlot() const;
  uint8_t getCurrentSlot() const;
  uint16_t getCycleDuration() const { return cycleDuration; }
  float getUpdateRate() const { return 1000.0f / cycleDuration; }
};

#endif