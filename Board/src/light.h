#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>



class Light {
    public:
        Light(uint8_t data_pin, uint8_t num_leds);

        typedef enum {
            emOneColor = 0, // Один цвет
            emLights, // Фары
            emPolice, // Мигалка(два светодиода)
            emPoliceAll, // Мигалка(все светодиоды)
            emRainbow, // Радуга
        } EffectMode;

        void begin();
        void update();
        void setOn(bool state);
        void setEffectByIndex(uint8_t index);
        void setEffect(EffectMode mode);
        void setBrightness(uint8_t bright);
        void setEffectColor(uint8_t index);
        void setLightsPulse(bool state);
        uint8_t getEffectIndex() {return static_cast<int>(mode_);}
        uint8_t getBrightness() {return leds_.getBrightness();}
        uint8_t getEffectColor() {return color_index_;};
        int getEffectSpeed() {return speed_;}
    
    private:
        uint8_t data_pin_;
        uint8_t num_leds_;
        bool is_on;
        uint8_t brightness_;
        uint8_t color_index_;
        int speed_;
        bool is_updated;
        bool is_pulse;
        uint8_t idex_;
        uint8_t idex;
        uint8_t thishue;
        uint8_t thissat;

        const uint32_t color_pallete_[13] = {
            leds_.Color(255, 0, 0),
            leds_.Color(255, 0, 255),
            leds_.Color(128, 0, 128),
            leds_.Color(86, 0, 111),
            leds_.Color(2, 0, 111),
            leds_.Color(0, 100, 255),
            leds_.Color(0, 128, 128),
            leds_.Color(0, 128, 0),
            leds_.Color(0, 255, 15),
            leds_.Color(251, 255, 0),
            leds_.Color(255, 173, 0),
            leds_.Color(255, 87, 0),
            leds_.Color(0, 243, 255)
        };

        EffectMode mode_;
        
        Adafruit_NeoPixel leds_;



        bool safeDelay(int del_time);
        int antipodal_index(int i);
};



#endif // _LIGHT_H_