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
  /**
   * @brief Constructor, create TDMA object
   * @param deviceID ID of the current device so it can be sorted
   * @param totalDevices Max devices in the system to split timing equally
   * @param slotWindow Window to transmit data
   * @param slotMs Total time for each slot. E.g. 10 devices = 100ms slotMs
   */
  TDMAScheduler(uint8_t deviceID, uint8_t totalDevices, uint16_t slotWindow, uint16_t slotMs);

  /**
   * @brief Interrupt from PPS to signal start of slot
   */
  void onPPSInterrupt(unsigned long ppsMicros);

  /**
   * @brief Reset and start a new cycle
   */
  void update();

  /**
   * @brief Check if it is device's turn to transmit
   * @return true if it is time to transmit
   */
  bool canTransmit() const;

  /**
   * @brief Get slot start time in microsecond
   */
  unsigned long getSlotStart() const;

    /**
   * @brief Get slot end time in microsecond
   */
  unsigned long getSlotEnd() const;

  /**
   * @brief Get cycle start time in microsecond
   */
  unsigned long getCycleStart() const;

  /**
   * @brief Get transmit window end time in microsecond
   */
  unsigned long getTransmitWindowEnd() const;


private:
  uint8_t _deviceID;
  uint8_t _totalDevices;
  unsigned long _slotDuration;
  unsigned long _slotWindow;
  unsigned long _cycleStart;
  bool _newCycle;
  
};

#endif

