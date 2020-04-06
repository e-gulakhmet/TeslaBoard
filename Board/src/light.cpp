#include <light.h>



Light::Light(uint8_t data_pin, uint8_t num_leds)
    : data_pin_(data_pin)
    , num_leds_(num_leds)
    , brightness_(50)
    , color_(0)
    , is_updated(false)
    , thishue(0)
    , thissat(255)
    , mode_(emOneColor)
    , speed_(esFreeze)
    , leds_(Adafruit_NeoPixel(num_leds_, data_pin_, NEO_GRB + NEO_KHZ800))
    {

    }



void Light::begin() {
    leds_.begin();
    leds_.setBrightness(brightness_);
    leds_.clear();
    leds_.show();
}



void Light::update() {
    switch (mode_) {
        case emOneColor: {
            if (!is_updated) {
                leds_.fill(255, 0, num_leds_);
                leds_.show();
                is_updated = true;
            }
            break;
        }
        
        case emLights: {
            if (!is_updated) {
                leds_.fill(leds_.Color(255, 255, 255), 0, num_leds_);
                leds_.show();
                is_updated = true;
            }
            break;
        }
        
        case emPolice: {
            idex_++;
            if (idex_ >= num_leds_) {
                idex_ = 0;
            }
            int idex_R = idex_;
            int idex_B = antipodal_index(idex_R);
            for (int i = 0; i < num_leds_; i++ ) {
                if (i == idex_R) {
                    leds_.setPixelColor(i, leds_.Color(255, 0, 0));
                }
                else if (i == idex_B) {
                    leds_.setPixelColor(i, leds_.Color(0, 0, 255));
                }
                else {
                    leds_.setPixelColor(i, leds_.ColorHSV(0, 0, 0));
                }
            }
            leds_.show();
            if (safeDelay(20)) return;
            break;
        }
        
        case emPoliceAll: {
            idex++;
            if (idex >= num_leds_) {
                idex = 0;
            }
            int idexR = idex;
            int idexB = antipodal_index(idexR);
            leds_.setPixelColor(idexR, leds_.Color(255, 0, 0));
            leds_.setPixelColor(idexB, leds_.Color(0, 0, 255));
            leds_.show();
            if (safeDelay(20)) return;
            break;
        }
        
        case emRainbow: {
            static uint16_t ihue = 0;
            ihue += 500;
            if (ihue > 65536) {
                ihue = 0;
            }
            for (int idex = 0 ; idex < num_leds_; idex++ ) {
                leds_.setPixelColor(idex, leds_.ColorHSV(ihue, thissat, 100));
            }
            leds_.show();
            if (safeDelay(30)) return;
            break;
        }
    }
}



void Light::setEffectMode(EffectMode mode) {
    mode_ = mode;
}



bool Light::safeDelay(int del_time) {
    static bool change_flag;
    uint32_t this_time = millis();
    while (millis() - this_time <= del_time) {
        if (change_flag) {
        change_flag = false;
        return true;
        }
    }
    return false;
}

int Light::antipodal_index(int i) {
    int iN = i + num_leds_ / 2;
    if (i >= num_leds_ / 2) {
        iN = (i + num_leds_ / 2) % num_leds_;
    }
    return iN;
}