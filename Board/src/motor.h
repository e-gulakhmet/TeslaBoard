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
        Motor(uint8_t motor_pin);

        typedef enum {
            mOff,
            mComfort,
            mNormal,
            mSport
        } Mode;

        void begin();
        void update();
        void setPower(uint8_t value); // Установка мощности мотора(0 - 255)
        void setMode(Mode mode); // Выбор режима
        void switchMode(bool clockwice);
        String getModeName();

    private:
        uint8_t motor_pin_;
        uint8_t power_;
        String mode_name_[4] = {"Off", "Comfort", "Normal", "Sport"};
        
        uint16_t motor_spec_[4][3] = {
            // 0 - Максимальное изменение(0 - 255),
            // 1 - Задержка между изменениями в мс,
            // 2 - Максимальное значение частоты работы мотора(800 - 2300)
            {255, 0, 800},
            {10, 250, 1200},
            {50, 100, 1800},
            {254, 0, 2300}
        };

        unsigned long motor_delay_;

        Mode mode_;
        Servo motor_;
};


#endif // _MOTOR_H_