#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "motor.h"

RF24 radio(9,10);
Motor motor;

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

byte got_data[4];
uint8_t power;
bool double_click;


void setup(){
  Serial.begin(9600);

  motor.init(3);
  
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);     //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах
  radio.openReadingPipe(1,pipes[0]);      //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}



void loop() {
  byte pipeNo;
  while(radio.available(&pipeNo)){    // слушаем эфир со всех труб
    radio.read( &got_data, sizeof(got_data) );         // чиатем входящий сигнал
    motor.update();

    power = got_data[0];  // Данные о положение потенциометра
    double_click = got_data[1]; // Была ли кнопка нажата два раза

    motor.setPower(power); // Настраиваем скорость

    if (double_click) { // Если кнопка на пульте была нажата два раза
      motor.switchMainMode(true); // Выбераем следущий режим
    }      
  }
  
  // Если данные от передатчика не приходят
  motor.setPower(0); // Выключаем мотор
}