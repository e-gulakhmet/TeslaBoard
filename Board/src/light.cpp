#include <light.h>



Light::Light(uint8_t data_pin, uint8_t num_leds)
    : data_pin_(data_pin)
    , num_leds_(num_leds)
    , brightness_(50)
    , color_(0)
    , is_updated(false)
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
        case emOneColor:
            if (!is_updated) {
                leds_.fill(200, 0, num_leds_);
                leds_.show();
                is_updated = true;
            }

            break;
    }
}



void Light::setEffectMode(EffectMode mode) {
    mode_ = mode;
}
