#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Arduino.h>
#include <FastLED.h>


class Light{
    public:
        Light(uint8_t pin);
        void begin();
        void oneColor();
    
    private:
        uint8_t pin_;
        CRGB leds_[32];
};




#endif // _LIGHT_H_