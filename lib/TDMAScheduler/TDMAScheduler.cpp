#include "TDMAScheduler.h"

TDMAScheduler::TDMAScheduler(uint8_t deviceID, uint8_t numDevices, uint16_t slotMs)
  : myDeviceID(deviceID), 
    slotDuration(slotMs),
    totalSlots(numDevices),
    transmitWindow(50),
    lastGPSSecond(0),
    lastPPSMicros(0),
    synchronized(false) {
  
  cycleDuration = slotDuration * totalSlots;
  
  Serial.println("TDMA Scheduler with PPS:");
  Serial.println("  Device ID: " + String(myDeviceID));
  Serial.println("  Slot duration: " + String(slotDuration) + "ms");
  Serial.println("  Cycle duration: " + String(cycleDuration) + "ms");
}

void TDMAScheduler::updateWithPPS(uint32_t ppsMicros, uint32_t gpsSecond) {
  lastPPSMicros = ppsMicros;
  lastGPSSecond = gpsSecond;
  synchronized = true;
}

uint32_t TDMAScheduler::getCurrentTimeMicros() const {
  if (!synchronized) {
    return 0;
  }
  
  // Time elapsed since last PPS pulse (in microseconds)
  uint32_t currentMicros = micros();
  uint32_t elapsedMicros = currentMicros - lastPPSMicros;
  
  // Handle micros() rollover (every ~71 minutes)
  if (currentMicros < lastPPSMicros) {
    elapsedMicros = (0xFFFFFFFF - lastPPSMicros) + currentMicros + 1;
  }
  
  // Current GPS time = last whole second + elapsed microseconds
  return (lastGPSSecond * 1000000UL) + elapsedMicros;
}

uint32_t TDMAScheduler::getCurrentTimeMillis() const {
  return getCurrentTimeMicros() / 1000;
}

uint32_t TDMAScheduler::getCycleStartTime() const {
  uint32_t currentTime = getCurrentTimeMillis();
  return (currentTime / cycleDuration) * cycleDuration;
}

uint32_t TDMAScheduler::getMySlotStartTime() const {
  uint32_t cycleStart = getCycleStartTime();
  uint32_t slotOffset = (myDeviceID - 1) * slotDuration;
  return cycleStart + slotOffset;
}

bool TDMAScheduler::canTransmit() const {
  if (!synchronized) {
    return false;
  }
  
  uint32_t currentTimeMicros = getCurrentTimeMicros();
  uint32_t slotStartMicros = getMySlotStartTime() * 1000UL;  // Convert to microseconds
  uint32_t slotEndMicros = slotStartMicros + (transmitWindow * 1000UL);
  
  return (currentTimeMicros >= slotStartMicros && currentTimeMicros < slotEndMicros);
}

int32_t TDMAScheduler::getTimeUntilMySlot() const {
  if (!synchronized) {
    return -1;
  }
  
  uint32_t currentTime = getCurrentTimeMillis();
  uint32_t slotStart = getMySlotStartTime();
  
  if (currentTime < slotStart) {
    return slotStart - currentTime;
  } else {
    // Slot already passed, next one is in next cycle
    uint32_t nextCycleStart = getCycleStartTime() + cycleDuration;
    uint32_t nextSlotStart = nextCycleStart + (myDeviceID - 1) * slotDuration;
    return nextSlotStart - currentTime;
  }
}

uint8_t TDMAScheduler::getCurrentSlot() const {
  if (!synchronized) {
    return 0;
  }
  
  uint32_t currentTime = getCurrentTimeMillis();
  uint32_t cycleStart = getCycleStartTime();
  uint32_t timeInCycle = currentTime - cycleStart;
  return timeInCycle / slotDuration;
}