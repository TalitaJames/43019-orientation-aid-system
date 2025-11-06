#pragma once
#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

//  EXPECT TARGET <meters> METERS, <hour> O'CLOCK
class TTS {
public:
  bool begin();                                     // call once in setup()
  void sayReport(uint16_t bearingDeg, uint16_t m);  // blocking

private:
  Talkie voice;

  static uint8_t bearingToHour(uint16_t deg);       // 0 to 360 -> 1..12
  static void sayDigit(Talkie& v, uint8_t d);       // 0 to 9
  static void sayDigits(Talkie& v, uint16_t n);     //  180 -> "ONE EIGHT ZERO"
  static void sayHour(Talkie& v, uint8_t h);        // adds “O'CLOCK”
};