#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <Arduino.h>
#include <Servo.h>

/*
Класс отвечающий за работу мотора.
Здесь происходит управление скоростью и режимами работы мотора.
*/


class Motor {
    public:
        Motor();

        typedef enum {
            mOff = -1,
            mComfort,
            mNormal,
            mSport
        } Mode;

        void init(uint8_t motor_pin);
        void update();
        void setPower(uint8_t value); // Установка мощности мотора(0 - 255)
        void setMode(uint8_t index); // Выбор режима
        void switchMainMode(bool clockwice);
        String getModeName();

    private:
        uint8_t motor_pin_;
        uint8_t power_;
        String mode_name_[4] = {"Off", "Comfort", "Normal", "Sport"};
        Mode mode_;
        Servo motor_;
};


#endif // _MOTOR_H_