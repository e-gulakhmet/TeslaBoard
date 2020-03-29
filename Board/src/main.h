#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t MOTOR_PIN = 3;
const uint8_t RADIO_CS_PIN = 9;
const uint8_t RADIO_DO_PIN = 10;
const uint8_t BUTT_PIN = A0;
const uint8_t DATA_PIN = 5;

typedef enum {
    emOneColor,
    emLights,
    emPolice,
    emRainbow,
} LightsMode;


#endif // _MAIN_H_