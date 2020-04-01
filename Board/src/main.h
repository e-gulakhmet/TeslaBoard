#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t MOTOR_PIN = 3; // Пин мотора
const uint8_t RADIO_CS_PIN = 9; // Пин радио
const uint8_t RADIO_CSN_PIN = 10; // Пин радио
const uint8_t BUTT_PIN = A0; // Пин кнопки
const uint8_t LEDS_PIN = 5; // Пин ленты
const uint8_t NUM_LEDS = 32; // Количество светодиодов
const uint8_t TEMP_PIN = A3; // Пин датчика температуры


typedef enum { // Режимы подсветки
    emOneColor, // Один цвет
    emLights, // Фары
    emPolice, // Мигалка(два светодиода)
    emPoliceAll, // Мигалка(все светодиоды)
    emRainbow, // Радуга
} LightsMode;


#endif // _MAIN_H_