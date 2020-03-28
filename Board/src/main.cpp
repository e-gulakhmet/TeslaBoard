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
bool is_send = true;
unsigned long send_timer;
unsigned long radio_timer;
bool is_connect;



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


  if (!is_setting) {
    byte pipeNo;
    if (radio.available(&pipeNo)) { // слушаем эфир со всех труб
      radio.read(&got_data, sizeof(got_data)); // читаем входящий сигнал
      if (millis() - send_timer > 2000) { // Отправляем данные обратно каждые 2 секунды
        radio.writeAckPayload(1, &send_data, sizeof(send_data));
        send_timer = millis();
      }

      radio_timer = millis(); // Сбрасываем таймер подключения

      motor.setPower(got_data[0]);  // Данные о положение потенциометра
      motor.setMode(got_data[1]); // Данные о выбранном режиме

      send_data[0] = 20; // Отправляем данные о заряде
      send_data[1] = 30; // Отправляем данные о температуре

    }

    if (millis() - radio_timer > 3000) { // Если данные от пульта не поступали больше 3 сикунд
      // Выключаем мотор
      motor.setMode(Motor::mOff);
      motor.setPower(0);
      radio_timer = millis();
    }
  }
  else { // В режиме настроек включаем спорт режим
    motor.setMode(Motor::mSport); // Максимальная чувствительность
    if (digitalRead(BUTT_PIN) == 0)
      motor.setPower(255);
    else
      motor.setPower(0);
  }


  motor.update();
}