#include <SPI.h>
#include <LoRa.h>

#define SS   5
#define RST  14
#define DIO0 4

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("LoRa Receiver Test");
  
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Match transmitter parameters
  LoRa.setTxPower(20);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x34);  // Try LoRaWAN default first
  LoRa.disableInvertIQ();
  LoRa.enableCrc();
  
  Serial.println("LoRa Initialized!");
  Serial.println("Listening for packets...");
}

void loop() {
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