#include "TDMAScheduler.h"

TDMAScheduler::TDMAScheduler(uint8_t deviceID, uint8_t totalDevices, uint16_t slotWindow, uint16_t slotMs)
  : _deviceID(deviceID),
    _totalDevices(totalDevices),
    _slotDuration(slotMs * 1000UL),
    _slotWindow(slotWindow * 100UL),
    _cycleStart(0),
    _newCycle(false)
{  
}

void TDMAScheduler::onPPSInterrupt(unsigned long ppsMicros) {
  _cycleStart = ppsMicros;
  _newCycle = true;
}

void TDMAScheduler::update() {
    if (_newCycle) {
        _newCycle = false;
    }
}

// Determine if we are inside our slot window
bool TDMAScheduler::canTransmit() const {
    if (_cycleStart == 0) return false;  // haven't synced yet

    unsigned long now = micros();
    unsigned long elapsed = now - _cycleStart;

    // Compute this device’s window:
    unsigned long start = (_deviceID - 1) * _slotDuration;
    unsigned long end = start + _slotWindow;

    // We can transmit if we're within the active window (not in guard time)
    return (elapsed >= start && elapsed < end);
}

unsigned long TDMAScheduler::getSlotStart() const {
    return (_deviceID - 1) * _slotDuration;
}

unsigned long TDMAScheduler::getSlotEnd() const {
    return (_deviceID) * _slotDuration;
}

unsigned long TDMAScheduler::getTransmitWindowEnd() const {
  return (_deviceID - 1) * _slotDuration + _slotWindow;
}

unsigned long TDMAScheduler::getCycleStart() const {
    return _cycleStart;
}

