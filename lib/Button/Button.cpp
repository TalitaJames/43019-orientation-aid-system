#include "Button.h"

boolean speakReading = false;
int isrcount = 0;

/**
 * @brief Sets up interupts and functionality for the button
 *
 */
void buttonSetup(){
    // pinMode(LED_PIN, OUTPUT); //debug led
    // ledStatus = false;
    // digitalWrite(LED_PIN, ledStatus);

    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    interrupts(); //enables use of interupts
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, CHANGE);
    speakReading = false;

    Serial.println("Button Setup Complete");
}

/**
 * @brief When the button is pressed, this code will interupt the loop
 *
 */
void IRAM_ATTR buttonInterrupt(){
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
    isrcount++;
    speakReading = true;

    Serial.print("Button Interupt ");
    Serial.println(isrcount);
    // delay(1*1000);
    // tts.sayReport(0,10);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, CHANGE);
}

/**
 * @brief Displays data about the button and status
 *
 */
void debugButton(){
    int btnStatus = !((boolean) digitalRead(BUTTON_PIN));
    Serial.print("btnStatus: ");
    Serial.print(btnStatus);
    // Serial.print("\tledStatus: ");
    // Serial.print(ledStatus);

    if (btnStatus){
        Serial.print("\tButton Pressed");
    }

    Serial.println();
}
