#ifndef _POWER_H_
#define _POWER_H_

#include <Arduino.h>

/*
Класс возвращающий данные о напряжении батареи.
*/


class Power{
    public:
        Power(uint8_t volt_pin);
        void update(); // Собираем информацию
        bool isCharging() {return is_charging;}
        float getVoltage() {return voltage_;}; // Функция получения напряжения
        uint8_t getProcent() {return procent_;}; // Функция получаения процента заряда
        void setMinVolt(float min_volt); // Функция установки максимального напряжения
        void setMaxVolt(float max_volt); // Функция установки минимального напряжения
    
    private:
        float readFilterVcc(); // Фильтр для снижения количества шупом
        float readAnalog(); // Функция считывания опорного напряжения

        uint8_t volt_pin_;
        bool is_charging;
        float voltage_;
        uint8_t procent_;
        float min_volt_;
        float max_volt_;
};


#endif // _POWER_H_