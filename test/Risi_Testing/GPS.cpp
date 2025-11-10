/////// getting lon and lat //////////////

#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

HardwareSerial SerialGPS(2);     // UART2
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nGPS parser test...");
  SerialGPS.begin(9600, SERIAL_8N1, 32, 33); // RX=32, TX=33
}

void loop() {
  while (SerialGPS.available()) {
    gps.encode(SerialGPS.read());
  }

  // Print once per second when we have valid data
  static uint32_t last = 0;
  if (millis() - last > 1000) {
    last = millis();

    if (gps.location.isValid()) {
      Serial.print("Lat: ");  Serial.print(gps.location.lat(), 6);
      Serial.print("  Lon: ");Serial.print(gps.location.lng(), 6);
      Serial.print("  Sats: ");Serial.print(gps.satellites.value());
      Serial.print("  HDOP: ");Serial.print(gps.hdop.value()/100.0, 2);
      Serial.print("  UTC: ");
      Serial.printf("%02d:%02d:%02d\n",
        gps.time.hour(), gps.time.minute(), gps.time.second());
    } else {
      Serial.println("Searching satellites... (no fix yet)");
    }
  }
}

/////////////////////// NMEA sentences /////////////////

// #include <HardwareSerial.h>
// HardwareSerial SerialGPS(2); // use UART2

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("\nStarting GPS test...");
  
//   // RX = 32, TX = 33
//   SerialGPS.begin(9600, SERIAL_8N1, 32, 33);
// }

// void loop() {
//   while (SerialGPS.available()) {
//     char c = SerialGPS.read();
//     Serial.write(c);
//   }
// }
