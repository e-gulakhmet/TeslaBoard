#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t MOTOR_PIN = 3;
const uint8_t RADIO_CS_PIN = 9;
const uint8_t RADIO_DO_PIN = 10;
const uint8_t BUTT_PIN = A0;
const uint8_t LEDS_PIN = 5;
const uint8_t NUM_LEDS = 32;
const uint8_t TEMP_PIN = A1

typedef enum {
    emOneColor,
    emLights,
    emPolice,
    emPoliceStrobe,
    emRainbow,
} LightsMode;


#endif // _MAIN_H_