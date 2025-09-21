/**
 * @file constants.h
 * @author Talita
 * @brief constant values and macros for entire system use
 * @date 2025-09-21
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// round number to an int
#define ROUND_2_INT(f) ((int)((f - ((int)f)) >= 0.5 ? (f + 0.5) : (f - 0.5)))

const int DEVICE_COUNT = 4; // number of total devices (boats, buoy)
const int ALARM_PROXIMITY = 100; // distance away a boat must be before alarm trigers [m]


#endif //CONSTANTS_H