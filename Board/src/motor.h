#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <Arduino.h>
#include <Servo.h>



class Motor {
    public:
        Motor();

        typedef enum {
            mComfort,
            mNormal,
            mSport
        } Mode;

        void init(uint8_t motor_pin);
        void update();
        void setPower(uint8_t value);
        void setMode(Mode mode);
        String getModeName();

    private:
        uint8_t motor_pin_;
        uint8_t power_;
        String mode_name_[3] = {"Comfort", "Normal", "Sport"};
        Mode mode_;
        Servo motor_;
};


#endif // _MOTOR_H_