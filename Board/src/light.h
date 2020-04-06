#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>



class Light {
    public:
        Light(uint8_t data_pin, uint8_t num_leds);
        void begin();
        void update();
        void oneColor() {setEffectMode(emOneColor);}

        typedef enum {
            esFreeze,
            esSlow,
            esNormal,
            esFast
        } EffectSpeed;

        typedef enum {
            emOneColor = 0, // Один цвет
            emLights, // Фары
            emPolice, // Мигалка(два светодиода)
            emPoliceAll, // Мигалка(все светодиоды)
            emRainbow, // Радуга
        } EffectMode;
    
    private:
        uint8_t data_pin_;
        uint8_t num_leds_;
        uint8_t brightness_;
        uint8_t color_;
        bool is_updated;

        EffectMode mode_;
        EffectSpeed speed_;

        Adafruit_NeoPixel leds_;

        void setEffectMode(EffectMode mode);
};



#endif // _LIGHT_H_