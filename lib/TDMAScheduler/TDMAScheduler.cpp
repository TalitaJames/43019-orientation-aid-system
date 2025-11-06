#include "TDMAScheduler.h"

TDMAScheduler::TDMAScheduler(uint8_t deviceID, uint8_t numDevices, uint16_t slotMs)
  : myDeviceID(deviceID),
    totalSlots(numDevices),
    slotDuration(slotMs * 1000UL),
    transmitWindow(slotMs * 800UL/1000UL), //80% of transmit slot
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

uint32_t TDMAScheduler::microsSincePPS() const {
  if (!synchronized) return 0;
  uint32_t now = micros();
  if (now >= lastPPSMicros)
    return now - lastPPSMicros;
  else
    return (0xFFFFFFFF - lastPPSMicros) + now + 1; // handle rollover
}

bool TDMAScheduler::canTransmit() const {
  if (!synchronized) {
    return false;
  }
  
  uint32_t delta = microsSincePPS();
  // guard: outside of current 1-second PPS frame
  if (delta >= 1000000UL) return false;
  uint32_t mySlotStart = (myDeviceID - 1) * slotDuration;
  uint32_t mySlotEnd = mySlotStart + transmitWindow;

  return (delta >= mySlotStart && delta < mySlotEnd);
  //uint32_t currentTimeMicros = getCurrentTimeMicros();
  //uint32_t slotStartMicros = getMySlotStartTime() * 1000UL;  // Convert to microseconds
  //uint32_t slotEndMicros = slotStartMicros + (transmitWindow * 1000UL);
  
  //return (currentTimeMicros >= slotStartMicros && currentTimeMicros < slotEndMicros);
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

/*
void IRAM_ATTR TDMAScheduler::onPPS() {
  lastPPSMicros = micros();  // timestamp as close to edge as possible
  newPPS = true;
}
  */