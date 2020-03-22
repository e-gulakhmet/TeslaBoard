#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "motor.h"
#include "main.h"

RF24 radio(RADIO_CS_PIN, RADIO_DO_PIN);
Motor motor(MOTOR_PIN);

byte got_data[4];
byte send_data[4];
uint8_t power;
bool butt_double_click;
bool butt_holded;
bool is_on = true;
bool is_setting;


// TODO: Отправлять данные только если они изменились



void setup(){
  Serial.begin(9600);

  pinMode(BUTT_PIN, INPUT_PULLUP);
  
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);     //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);      //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
  radio.writeAckPayload(1, &send_data, sizeof(send_data));

  motor.begin();

  if (digitalRead(A0) == 0) {
    is_setting = true;
  }
}



void loop() {
  byte pipeNo;
  while (radio.available(&pipeNo)) { // слушаем эфир со всех труб
    radio.read(&got_data, sizeof(got_data)); // читаем входящий сигнал
    radio.writeAckPayload(1, &send_data, sizeof(send_data));
   
    power = got_data[0];  // Данные о положение потенциометра
    butt_double_click = got_data[1]; // Была ли кнопка нажата два раза
    butt_holded = got_data[2]; // Информация об удержание кнопки


    send_data[0] = motor.getPower();
    send_data[1] = motor.getModeName();


    if (is_setting) {
      motor.setMode(Motor::mOff);
    }

    else {
      if (butt_double_click) // Если кнопка на пульте была нажата два раза
        motor.switchMode(true); // Выбераем следущий режим

      if (butt_holded) // Если кнопка была зажата в течении 1 секунды
        motor.setMode(Motor::mOff); // Выключаем или включаем управление мотором

      motor.setPower(power);
    }
  }

  // Если данные от передатчика не приходят
}