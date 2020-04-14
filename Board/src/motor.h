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
        void switchMode(bool clockwice); // Переключение текущего режима по часовой стрелке или против
        void setMaxTemp(uint8_t max_temp);

        void setEcoModeSpec(uint8_t delay, uint8_t max_power);

        void setNormalModeSpec(uint8_t delay, uint8_t max_power);

        uint8_t getModeIndex() {return mode_;} // Получаем номер текущего режима
        uint8_t getPower() {return power_;} // Получаем текущюю скорость
        uint8_t getTemp() {return temp_;} // Получаем данные о температуре драйвера
        uint8_t getMaxTemp() {return max_temp_;}

        uint8_t getEcoModeDelay() {return motor_spec_[mComfort].delay;}
        uint8_t getEcoModeMaxPower() {return motor_spec_[mComfort].max_power;}

        uint8_t getNormalModeDelay() {return motor_spec_[mNormal].delay;}
        uint8_t getNormalModeMaxPower() {return motor_spec_[mNormal].max_power;};        

    private:
        uint8_t motor_pin_;
        uint8_t temp_pin_;
        uint8_t power_;
        uint8_t temp_;
        uint8_t max_temp_;
        
        typedef struct {
            uint8_t change;
            uint8_t delay;
            uint16_t max_power;
        } MotorSpec;

        MotorSpec motor_spec_[4] = {
            // 0 - Максимальное изменение(0 - 255),
            // 1 - Задержка между изменениями в мс,
            // 2 - Максимальное значение частоты работы мотора(800 - 2300)
            {255, 0, 800},
            {10, 250, 1200},
            {30, 200, 1800},
            {254, 0, 2300}
        };

        unsigned long motor_delay_;

        Mode mode_;
        Servo motor_;
        OneWire oneWire_;
        DallasTemperature sensor_;
};


#endif // _MOTOR_H_