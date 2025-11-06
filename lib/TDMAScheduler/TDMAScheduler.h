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
public:

  TDMAScheduler(uint8_t deviceID, uint8_t totalDevices, uint16_t slotMs); //constructor 
  void onPPSInterrupt(unsigned long ppsMicros);
  void update();
  bool canTransmit() const;

  //debugging
  unsigned long getSlotStart() const;
  unsigned long getSlotEnd() const;
  unsigned long getCycleStart() const;

private:
  uint8_t _deviceID;
  uint8_t _totalDevices;
  unsigned long _slotDuration;
  unsigned long _cycleStart;
  bool _newCycle;
  
};

#endif

