#include "motor.h"


Motor::Motor(uint8_t motor_pin, uint8_t temp_pin)
    : motor_pin_(motor_pin)
    , temp_pin_(temp_pin)
    , power_(0)
    , temp_(0)
    , max_temp_(60)
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
    sensor_.requestTemperatures();
    temp_ = sensor_.getTempCByIndex(0);
}



void Motor::update() {
    static uint8_t value;
    static unsigned long timer;

    if (millis() - timer > 10000) { // Получаем данные о температуре каждые 10 сек
        timer = millis(); 
        sensor_.requestTemperatures();
        temp_ = sensor_.getTempCByIndex(0);
    }
    
    if (temp_ <= max_temp_) { // Если температура меньше 60 градусов
        // Управляем мотором
        if (int(power_ - value) > int(motor_spec_[mode_].change)) {
            if (millis() - motor_delay_ > motor_spec_[mode_].delay) {
                motor_delay_ = millis();
                value += motor_spec_[mode_].change;
            }
        }
        else {
            value = power_;
        }
    }
    else { // Если температура больше 60
        value = 0;
    }

    motor_.writeMicroseconds(map(value, 0, 255, 800, motor_spec_[mode_].max_power));
}



void Motor::setPower(uint8_t value) {
    if (power_ == value)
        return;
    
    power_ = constrain(power_, 0, 255);
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



void Motor::setMaxTemp(uint8_t max_temp) {
    if (max_temp_ == max_temp)
        return;

    if (max_temp <= 80)
        max_temp_ = max_temp;
    else
        max_temp_ = 80;
}