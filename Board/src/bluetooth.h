#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <Arduino.h>
#include <SoftwareSerial.h>



class Bluetooth {
    public:
        Bluetooth(uint8_t rx_pin, uint8_t tx_pin);
        void update();
        bool isConnect() {return is_connect_;}

        uint8_t getMotorMode() {return motor_data_.mode;}
        uint8_t getMotorPower() {return motor_data_.power;}

        bool isLightOn() {return light_data_.is_on;}
        uint8_t getLightBright() {return light_data_.bright;}
        uint8_t getLightMode() {return light_data_.mode;}
        uint8_t getLightColor() {return light_data_.color;}
        bool isLightBlink() {return light_data_.is_blink;}
        int getLightSpeed() {return light_data_.speed;}

    
    private:
        uint8_t rx_pin_;
        uint8_t tx_pin_;
        int bl_data_[6];     // массив численных значений после парсинга
        boolean is_connect_;

        typedef enum {
            smMain = 0,
            smLight
        } SettingMode;

        typedef struct {
            uint8_t power;
            uint8_t mode;
        } MotorData;

        typedef struct {
            bool is_on;
            uint8_t mode;
            uint8_t bright;
            uint8_t color;
            bool is_blink;
            int speed;
        } LightData;

        SettingMode sett_mode_;
        MotorData motor_data_;
        LightData light_data_;

        SoftwareSerial blSerial_;

        void parsing();
};



#endif // _BLUETOOTH_H_