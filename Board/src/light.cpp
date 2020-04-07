#include <light.h>



Light::Light(uint8_t data_pin, uint8_t num_leds)
    : data_pin_(data_pin)
    , num_leds_(num_leds)
    , brightness_(50)
    , color_index_(0)
    , speed_(0)
    , is_updated(false)
    , thishue(0)
    , thissat(255)
    , mode_(emOneColor)
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
    if (is_on) { // Если подсветка включена
        switch (mode_) {
            case emOneColor: { 
                if (!is_updated) { // Если уже показали цвет на ленте
                    // Не обновляем ее, чтобы не грузить ардуинку
                    leds_.fill(color_pallete_[color_index_], 0, num_leds_);
                    leds_.show();
                    is_updated = true;
                }
                break;
            }
            
            case emLights: {
                // По бокам и спереди огни всегда горят
                if (!is_updated) {
                    leds_.fill(leds_.Color(255, 255, 255), 0, num_leds_ - 6);

                }
                if (is_pulse) { // Если режим мигания включен
                    is_updated = true;
                    if (safeDelay(300))
                        return;

                    static bool show;
                    
                    show = !show;

                    //  Моргаем задними лампами
                    if (show) {
                        leds_.fill(leds_.Color(255,0, 0), num_leds_ - 6, 6);
                    }
                    else {
                        leds_.fill(leds_.Color(0, 0, 0), num_leds_ - 6, 6);
                    }
                    leds_.show();
                }
                else { // Если режим мигания выключен
                    if (!is_updated) { // Просто включаем задние диоды
                        leds_.fill(leds_.Color(255, 0, 0), num_leds_ - 6, 6);
                        leds_.show();
                        is_updated = true;
                    }
                }
                break;
            }
            
            case emPolice: {
                if (safeDelay(20)) 
                    return;

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
                break;
            }
            
            case emPoliceAll: {
                if (safeDelay(20)) 
                    return;

                idex++;
                if (idex >= num_leds_) {
                    idex = 0;
                }
                int idexR = idex;
                int idexB = antipodal_index(idexR);
                leds_.setPixelColor(idexR, leds_.Color(255, 0, 0));
                leds_.setPixelColor(idexB, leds_.Color(0, 0, 255));
                leds_.show();
                break;
            }
            
            case emRainbow: {
                if (safeDelay(10))
                    return;

                static uint16_t ihue = 0;
                
                ihue += 500;
                if (ihue > 65536) {
                    ihue = 0;
                }
                for (int idex = 0 ; idex < num_leds_; idex++ ) {
                    leds_.setPixelColor(idex, leds_.ColorHSV(ihue, thissat, 100));
                }
                leds_.show();
                break;
            }
        }
    }
    else {
        if (!is_updated) {
            leds_.clear();
            leds_.show();
            is_updated = true;
        }
    }
}



void Light::setOn(bool state) {
    if (is_on != state) {
        is_on = state;
        is_updated = false; // Говорим, что нужно обновить ленту
    }

}



void Light::setEffectByIndex(uint8_t index) {
    EffectMode m = static_cast<EffectMode>(index); 
    if (mode_ != m) {
        mode_ = m;
        is_updated = false;
    }    
}



void Light::setEffect(EffectMode mode) {
    if (mode_ == mode)
        return;

    is_updated = false; 
    mode_ = mode;
}



void Light::setBrightness(uint8_t bright) {
    if (brightness_ == bright)
        return;

    brightness_ = bright;
    is_updated = false;
    leds_.setBrightness(brightness_);
}



void Light::setEffectColor(uint8_t index) {
    if (color_index_ == index)
        return;
    
    color_index_ = index;
    is_updated = false;
}



void Light::setLightsBlink(bool state) {
    if (is_pulse == state)
        return;
    
    is_pulse = state;
    is_updated = false;
}



bool Light::safeDelay(int del_time) {
    static unsigned long timer;
    if (millis() - timer <= del_time)
        return true;

    return false;
    timer = millis();
}

int Light::antipodal_index(int i) {
    int iN = i + num_leds_ / 2;
    if (i >= num_leds_ / 2) {
        iN = (i + num_leds_ / 2) % num_leds_;
    }
    return iN;
}