#include "Button.h"

boolean ledStatus;

/**
 * @brief Sets up interupts and functionality for the button
 *
 */
void buttonSetup(){
    pinMode(LED_PIN, OUTPUT); //debug led
    ledStatus = false;
    digitalWrite(LED_PIN, ledStatus);

    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // interrupts(); //enables use of interupts
    // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, RISING);

    Serial.println("Button Setup Complete");
}

/**
 * @brief When the button is pressed, this code will interupt the loop
 *
 */
void buttonInterrupt(){
    if (!digitalRead(BUTTON_PIN)){
        Serial.println("Button Pressed");
        digitalWrite(LED_PIN, ledStatus);
        ledStatus != ledStatus;
    }
}

/**
 * @brief Displays data about the button and status
 *
 */
void debugButton(){
    int btnStatus = !((boolean) digitalRead(BUTTON_PIN));
    Serial.print("btnStatus: ");
    Serial.print(btnStatus);
    Serial.print("\tledStatus: ");
    Serial.print(ledStatus);

    if (btnStatus){
        Serial.print("\tButton Pressed");
        digitalWrite(LED_PIN, ledStatus);
        ledStatus != ledStatus;
    }

    Serial.println();
}
