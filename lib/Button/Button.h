/**
 * @file button.h
 * @author Talita
 * @brief an interupt for the button
 * @date 2025-10-16
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <DeviceConfig.h>

#define BUTTON_PIN (DeviceConfig::BUTTON_PIN)

extern bool speakReading; // boolean flag to indicate speaker should talk

/**
 * @brief Sets up interupts and functionality for the button
 *
 */
void buttonSetup();

/**
 * @brief When the button is pressed, this code will interupt the loop
 *
 */
void IRAM_ATTR buttonInterrupt();

/**
 * @brief Displays data about the button and status
 *
 */
void debugButton();


#endif // BUTTON_H