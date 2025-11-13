/**
 * @file TTS.h
 * @author Milo
 * @brief Provides text-to-speech functionality using the Talkie library.
 * @date 2025-10-20
 * 
 * This module uses the Talkie library for generating spoken audio output 
 * from pre-defined English vocabulary. It enables the device to issue 
 * spoken alerts and distance reports. Custom phrases can be built by 
 * combining the provided vocabulary words.
 */

#pragma once
#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

/**
 * @class TTS
 * @brief Handles spoken feedback and user alerts using Talkie speech synthesis.
 */
class TTS {
public:
  /**
   * @brief Initialize the TTS system and speech engine.
   * @return True if initialization succeeds.
   */
  bool begin();

  /**
   * @brief Announce target distance and bearing to user.
   * 
   * Spoken output format:
   * `"EXPECT TARGET <meters> METERS, <hour> O'CLOCK"`
   * 
   * @param bearingDeg Bearing angle in degrees (0–359).
   * @param m Target distance in meters.
   */
  void sayReport(uint16_t bearingDeg, uint16_t m);

  /**
   * @brief Speak a warning message (“WARNING”).
   */
  void sayWarning();

  /**
   * @brief Speak an invalid/no-target message (“NO TARGET”).
   */
  void sayInvalid();

private:
  Talkie voice;  ///< Talkie speech engine instance.

  /**
   * @brief Convert bearing in degrees (0–360) to clock position (1–12).
   * @param deg Bearing angle in degrees.
   * @return Hour value (1–12).
   */
  static uint8_t bearingToHour(uint16_t deg);

  /**
   * @brief Speak a single digit (0–9).
   * @param v Reference to Talkie voice instance.
   * @param d Digit to speak.
   */
  static void sayDigit(Talkie& v, uint8_t d);

  /**
   * @brief Speak a multi-digit number (e.g., 180 → “ONE EIGHT ZERO”).
   * @param v Reference to Talkie voice instance.
   * @param n Number to speak.
   */
  static void sayDigits(Talkie& v, uint16_t n);

  /**
   * @brief Speak the hour value and append “O'CLOCK”.
   * @param v Reference to Talkie voice instance.
   * @param h Hour value (1–12).
   */
  static void sayHour(Talkie& v, uint8_t h);
};
