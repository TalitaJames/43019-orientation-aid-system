#include "Button.h"

boolean speakReading = false;
int isrcount = 0;

/**
 * @brief Sets up interupts and functionality for the button
 *
 */
void buttonSetup(){
    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    //enables use of interupts and attach the button to its isr
    interrupts();
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, CHANGE);
    speakReading = false;

    Serial.println("Button Setup Complete");
}

/**
 * @brief When the button is pressed, this code will interupt the loop
 *
 */
void IRAM_ATTR buttonInterrupt(){
    speakReading = true;
}

/**
 * @brief Displays data about the button and status
 *
 */
void debugButton(){
    int btnStatus = !((boolean) digitalRead(BUTTON_PIN));
    Serial.print("btnStatus: ");
    Serial.print(btnStatus);

    if (btnStatus){
        Serial.print("\tButton Pressed");
    }

    Serial.println();
}
