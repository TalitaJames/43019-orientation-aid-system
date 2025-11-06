#include "Button.h"

boolean ledStatus;

/**
 * @brief Sets up interupts and functionality for the button
 *
 */
void buttonSetup(){
    pinMode(LED_PIN, OUTPUT); //debug led
    ledStatus = false;
    digitalWrite(LED_PIN, LOW);

    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    interrupts(); //enables use of interupts
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, RISING);

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

void debugButton(){
    int btnStatus = digitalRead(BUTTON_PIN);
    Serial.print("btnStatus: ");
    Serial.print(btnStatus);
    Serial.print("\tledStatus: ");
    Serial.println(ledStatus);

    if (!digitalRead(BUTTON_PIN)){
        Serial.println("Button Pressed");
        digitalWrite(LED_PIN, ledStatus);
        ledStatus != ledStatus;
    }
}
