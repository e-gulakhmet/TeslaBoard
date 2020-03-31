#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <Arduino.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*
Класс отвечающий за работу мотора.
Здесь происходит управление скоростью и режимами работы мотора.
*/


class Motor {
    public:
        Motor(uint8_t motor_pin, uint8_t temp_pin);

        typedef enum {
            mOff,
            mComfort,
            mNormal,
            mSport
        } Mode;

        void begin();
        void update();
        void setPower(uint8_t value); // Установка мощности мотора(0 - 255)
        void setMode(uint8_t mode); // Выбор режима
        void switchMode(bool clockwice);
        uint8_t getModeName() {return mode_;}
        uint8_t getPower() {return power_;}

    private:
        uint8_t motor_pin_;
        uint8_t temp_pin_;
        uint8_t power_;
        uint8_t temp_;
        
        uint16_t motor_spec_[4][3] = {
            // 0 - Максимальное изменение(0 - 255),
            // 1 - Задержка между изменениями в мс,
            // 2 - Максимальное значение частоты работы мотора(800 - 2300)
            {255, 0, 800},
            {10, 250, 1200},
            {50, 150, 1800},
            {254, 0, 2300}
        };

        unsigned long motor_delay_;

        Mode mode_;
        Servo motor_;
        OneWire oneWire_;
        DallasTemperature sensor_;
};


#endif // _MOTOR_H_