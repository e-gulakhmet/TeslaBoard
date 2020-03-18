
#include <Arduino.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

Servo motor;

RF24 radio(9,10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(){
  Serial.begin(9600); //открываем порт для связи с ПК
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

  motor.attach(3);
}

void loop() {
    byte pipeNo, gotByte[4];                          
    while( radio.available(&pipeNo)){    // слушаем эфир со всех труб
      radio.read( &gotByte, sizeof(gotByte) );         // чиатем входящий сигнал

      Serial.print("Recieved: "); Serial.println(gotByte[0]);
      int val = map(gotByte[0], 0, 255, 800, 2300);
      motor.writeMicroseconds(val);
   }
}