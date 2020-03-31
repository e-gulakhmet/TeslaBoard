#include "motor.h"


Motor::Motor(uint8_t motor_pin, uint8_t temp_pin)
    : motor_pin_(motor_pin)
    , temp_pin_(temp_pin)
    , power_(0)
    , motor_delay_(0)
    , mode_(mComfort)
    , oneWire_(temp_pin_)
    , sensor_(&oneWire_)
    {
    }



void Motor::begin() {
    motor_.attach(motor_pin_);
    motor_.writeMicroseconds(800);
    sensor_.begin();
}



void Motor::update() {
    static uint8_t value;
    static unsigned long timer;

    if (millis() - timer > 10000) {
        sensor_.requestTemperatures();
        temp_ = sensor_.getTempCByIndex(0);
    }
    
    if (temp_ <= 90) {
        if (int(power_ - value) > int(motor_spec_[mode_][0])) {
            if (millis() - motor_delay_ > motor_spec_[mode_][1]) {
                motor_delay_ = millis();
                value += motor_spec_[mode_][0];
            }
        }
        else {
            value = power_;
        }
    }
    else {
        value = 0;
    }

    Serial.print(mode_); Serial.print("    "); Serial.println(value);
    motor_.writeMicroseconds(map(value, 0, 255, 800, motor_spec_[mode_][2]));
}



void Motor::setPower(uint8_t value) {
    if (power_ == value)
        return;
    
    power_ = value;
}



void Motor::setMode(uint8_t mode) {
    if (power_ <= 10)
        mode_ = static_cast<Mode>(mode);
}



void Motor::switchMode(bool clockwise) { // Переключение режимов
    if (power_ <= 10) { // Если курок газа отпущен
        int n = static_cast<int>(mode_);

        n += clockwise ? 1 : -1; // Если по часовой стрелке, то ставим следующий

        if ( n > 3) {
            n = 1;
        }
        if ( n < 1 ) {
            n = 1;
        }

        mode_ = static_cast<Mode>(n);        
    }
}