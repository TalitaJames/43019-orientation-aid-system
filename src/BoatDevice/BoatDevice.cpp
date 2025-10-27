#include <Arduino.h>
#include <DeviceConfig.h>

DeviceConfig config;

void setup () {
    //startup procedure
    Serial.begin(115200);
    config.begin();
    //wait for command to start race  
}

void loop () {
    String name = config.getDeviceName();

    if (Serial.available()){
        Serial.println("\nName is :");
        Serial.println(name);
    }
    //receive GPS data, send when its their turn

    //calculate euclidean distance for nearby boats

    //initiate beeper for boat 

    //collect buoy data, and calculate euclidean distance and compare to IMU readings.

    //prepare 'sentence' to output for spoken readings. 

    //if button pressed, output reading

    //if boat leaves target buoy range and passes global minima, switch to next target buoy. 

    //shutdown procedure
}