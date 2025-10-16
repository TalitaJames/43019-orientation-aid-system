#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

#define GPS_RX_PIN  16
#define GPS_TX_PIN  17
#define GPS_BAUD    9600


//will need to interpret NMEA string readings. have a look at what risi has done later. 

class GPSManager {
private:
  TinyGPSPlus gps;
  HardwareSerial* gpsSerial;


};

#endif 