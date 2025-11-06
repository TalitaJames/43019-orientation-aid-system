#include "DeviceRegistry.h"

DeviceRegistry::DeviceRegistry() {
  for (int i = 0; i < MAX_BOAT; i++) boats[i].id = INVALID_ID;
  for (int i = 0; i < MAX_BUOY; i++) buoys[i].id = INVALID_ID;
}

//
// ===== GPS HISTORY =====
//
void DeviceRegistry::addGPSHistory(float lat, float lon, uint32_t ts) {
  history[historyIndex] = {lat, lon, ts};
  historyIndex = (historyIndex + 1) % MAX_HISTORY_SEC;
  if (historyCount < MAX_HISTORY_SEC) historyCount++;
}

const GPSHistory* DeviceRegistry::getGPSHistoryArray() {
  return history;
}

uint8_t DeviceRegistry::getHistoryCount() {
  return historyCount;
}

//
// ===== BOAT MANAGEMENT =====
//
void DeviceRegistry::updateBoat(uint8_t id, float distance) {
  for (int i = 0; i < MAX_BOAT; i++) {
    if (boats[i].id == id) {
      boats[i].distance = distance;
      return;
    }
  }
  // Add new
  for (int i = 0; i < MAX_BOAT; i++) {
    if (boats[i].id == INVALID_ID) {
      boats[i].id = id;
      boats[i].distance = distance;
      return;
    }
  }
}

int DeviceRegistry::findBoat(uint8_t id) {
  for (int i = 0; i < MAX_BOAT; i++) {
    if (boats[i].id == id) return i;
  }
  return -1;
}

float DeviceRegistry::getBoatDistance(uint8_t id) {
  int idx = findBoat(id);
  return (idx >= 0) ? boats[idx].distance : -1.0f;
}

//
// ===== BUOY MANAGEMENT =====
//
void DeviceRegistry::updateBuoy(uint8_t id, float distance, float heading) {
  for (int i = 0; i < MAX_BUOY; i++) {
    if (buoys[i].id == id) {
      buoys[i].distance = distance;
      buoys[i].heading = heading;
      return;
    }
  }
  // Add new
  for (int i = 0; i < MAX_BUOY; i++) {
    if (buoys[i].id == INVALID_ID) {
      buoys[i].id = id;
      buoys[i].distance = distance;
      buoys[i].heading = heading;
      return;
    }
  }
}

int DeviceRegistry::findBuoy(uint8_t id) {
  for (int i = 0; i < MAX_BUOY; i++) {
    if (buoys[i].id == id+MAX_BOAT) return i;
  }
  return -1;
}

float DeviceRegistry::getBuoyID(uint8_t id) {
  int idx = findBuoy(id);
  return (idx >= 0) ? buoys[idx].id : -1.0f;
}

float DeviceRegistry::getBuoyDistance(uint8_t id) {
  int idx = findBuoy(id);
  return (idx >= 0) ? buoys[idx].distance : -1.0f;
}

float DeviceRegistry::getBuoyHeading(uint8_t id) {
  int idx = findBuoy(id);
  return (idx >= 0) ? buoys[idx].heading : -1.0f;
}