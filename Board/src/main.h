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
const uint8_t BL_RX = 6;
const uint8_t BL_TX = 7;


uint8_t color_pallete[13][3] = {
    {255, 0, 0},
    {255, 0, 255},
    {128, 0, 128},
    {86, 0, 111},
    {2, 0, 111},
    {0, 100, 255},
    {0, 128, 128},
    {0, 128, 0},
    {0, 255, 15},
    {251, 255, 0},
    {255, 173, 0},
    {255, 87, 0},
    {0, 243, 255}
};



#endif // _MAIN_H_