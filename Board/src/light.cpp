#include "light.h"


Light::Light(uint8_t pin)
    : pin_(pin)
    {

    }

void Light::begin() {
    FastLED.addLeds<WS2812B, 5, GRB>(leds_, 32).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(50);
}

void Light::oneColor() {
    fill_solid( &(leds_[0]), 32 /*number of leds*/, CRGB( 255, 255, 255) );
    FastLED.show();
}