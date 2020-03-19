#include "motor.h"


Motor::Motor()
    : power_(0)
    , mode_(mComfort)
    {

    }



void Motor::init(uint8_t motor_pin) {
    motor_pin_ = motor_pin;
    motor_.attach(motor_pin_);
}



void Motor::update() {
    switch (mode_) {
        case mComfort:
            int val = map(power_, 0, 255, 800, 1200);
            motor_.writeMicroseconds(val);
            break;
    }
}



void Motor::setPower(uint8_t value) {
    power_ = value;
}