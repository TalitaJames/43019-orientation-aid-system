/**
 * @file button.h
 * @author Talita
 * @brief an interupt for the button
 * @date 2025-10-16
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define BUTTON_PIN 12
// #define LED_PIN 22

extern bool speakReading;

void IRAM_ATTR buttonInterrupt();
void buttonSetup();
void debugButton();


#endif // BUTTON_H