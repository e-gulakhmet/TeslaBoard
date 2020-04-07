#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>



class Light {
    public:
        Light(uint8_t data_pin, uint8_t num_leds);

        typedef enum {
            emOff = -1,
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
        uint8_t idex_;
        bool is_updated;
        uint8_t idex;
        uint8_t thishue;
        uint8_t thissat;

        EffectMode mode_;
        
        Adafruit_NeoPixel leds_;

        bool safeDelay(int del_time);
        int antipodal_index(int i);
};



#endif // _LIGHT_H_