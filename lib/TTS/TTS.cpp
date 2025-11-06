#include "TTS.h"

bool TTS::begin() { return true; }

void TTS::sayReport(uint16_t bearingDeg, uint16_t m) {
  // EXPECT TARGET
  voice.say(sp4_EXPECT);
  voice.say(spPAUSE1);
  voice.say(sp4_TARGET);
  voice.say(spPAUSE1);

  // <meters> METERS
  sayDigits(voice, m);
  voice.say(sp2_METER);         
  voice.say(spPAUSE1);

  // <hour> O'CLOCK
  sayHour(voice, bearingToHour(bearingDeg));
}

void TTS::sayWarning() {
  voice.say(sp4_WARNING);         
  voice.say(spPAUSE1);
}

uint8_t TTS::bearingToHour(uint16_t deg) {
  deg %= 360;
  int h = (deg + 15) / 30;      // round to nearest 30°
  if (h == 0) h = 12;
  return (uint8_t)h;
}

void TTS::sayDigit(Talkie& v, uint8_t d) {
  static const uint8_t* t[] = {
    sp2_ZERO, sp2_ONE, sp2_TWO, sp2_THREE, sp2_FOUR,
    sp2_FIVE, sp2_SIX, sp2_SEVEN, sp2_EIGHT, sp2_NINE
  };
  v.say(t[d]);
}

void TTS::sayDigits(Talkie& v, uint16_t n) {
  // speak each digit (180 -> "ONE EIGHT ZERO")
  char buf[6];
  snprintf(buf, sizeof(buf), "%u", n);
  for (char* p = buf; *p; ++p) {
    sayDigit(v, (uint8_t)(*p - '0'));
    v.say(spPAUSE1);
  }
}

void TTS::sayHour(Talkie& v, uint8_t h) {
  switch (h) {
    case 1:  v.say(sp2_ONE);    break; case 2:  v.say(sp2_TWO);    break;
    case 3:  v.say(sp2_THREE);  break; case 4:  v.say(sp2_FOUR);   break;
    case 5:  v.say(sp2_FIVE);   break; case 6:  v.say(sp2_SIX);    break;
    case 7:  v.say(sp2_SEVEN);  break; case 8:  v.say(sp2_EIGHT);  break;
    case 9:  v.say(sp2_NINE);   break; case 10: v.say(sp2_TEN);    break;
    case 11: v.say(sp2_ELEVEN); break; default: v.say(sp2_TWELVE); break;
  }
  v.say(sp3_OCLOCK);
}