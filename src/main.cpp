#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>
 
HardwareSerial SerialGPS(2); // use UART2
TinyGPSPlus gps;
 
#define ss 5
#define rst 14
#define dio0 26
 
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\nStarting test...");
  
  // RX = 32, TX = 33
  SerialGPS.begin(9600, SERIAL_8N1, 32, 33);
 
  // Setup LoRa transceiver
  LoRa.setPins(ss, rst, dio0);
 
  while (!LoRa.begin(915E6)) {
    Serial.print(".");
    delay(500);
  }
 
  // Match transmitter parameters
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0xF3);
  LoRa.disableInvertIQ();
  LoRa.enableCrc();
 
  Serial.println("LoRa Initialized!");
  Serial.println("Listening for packets...");
}
 
void loop() {
  while (SerialGPS.available()) {
    char c = SerialGPS.read();
    //Serial.write(c);
    gps.encode(c);           // <--- This is what was missing!
  }
 
  // Once the GPS has valid fix and location, print it in decimal degrees
  if (gps.location.isUpdated()) {
    Serial.print("Location: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
  }
 
  int packetSize = LoRa.parsePacket();
 
  if (packetSize) {
    Serial.print("Received packet: '");
   
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
   
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}