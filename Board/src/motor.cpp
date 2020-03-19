#include "motor.h"


Motor::Motor()
    : power_(0)
    , mode_(mComfort)
    {

    }



void Motor::init(uint8_t motor_pin) {
    motor_pin_ = motor_pin;
    motor_.attach(motor_pin_);

    motor_.writeMicroseconds(2300);
    delay(2000);
    motor_.writeMicroseconds(800);
    delay(6000);
}



void Motor::update() {
    switch (mode_) {
        case mOff:{
            motor_.writeMicroseconds(800);
        } break;

        case mComfort:{
            int val = map(power_, 0, 255, 800, 1200);
            motor_.writeMicroseconds(val);
        } break;
        
        case mNormal:{
            int val = map(power_, 0, 255, 800, 1800);
            motor_.writeMicroseconds(val);
        } break;
        
        case mSport:{
            int val = map(power_, 0, 255, 800, 2300);
            motor_.writeMicroseconds(val);
        } break;
    }
}



void Motor::setPower(uint8_t value) {
    if (value > 255) {
        power_ = 255;
    }
    else if (value < 0) {
        power_ = 0;
    }
    else {
        power_ = value;
    }
}



void Motor::setMode(uint8_t index) {
    if (power_ == 0) {
        if (index > mSport) {
            mode_ = mSport;
        }
        else if (index < mComfort) {
            mode_ = mComfort;
        }
        mode_ = static_cast<Mode>(index);
    }
}



void Motor::switchMainMode(bool clockwice) { // Переключение режимов
    if (power_ == 0) { // Если курок газа отпущен
        int n = static_cast<int>(mode_);

        n += clockwice ? 1 : -1; // Если по часовой стрелке, то ставим следующий

        if ( n > 3) {
            n = 0;
        }
        if ( n < 0 ) {
            n = 0;
        }

        mode_ = static_cast<Mode>(n);        
    }
}