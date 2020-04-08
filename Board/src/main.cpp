#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

#include "motor.h"
#include "main.h"
#include "bluetooth.h"
#include "light.h"

RF24 radio(RADIO_CS_PIN, RADIO_CSN_PIN);
Motor motor(MOTOR_PIN, TEMP_PIN);
Light light(LEDS_PIN, NUM_LEDS);
Bluetooth blt(BL_RX, BL_TX);

byte got_data[3];
byte send_data[3];

bool is_setting;
unsigned long send_timer;
unsigned long radio_timer;



// TODO: Сделать настойку режимов мотора
// TODO: Настройка максмальной температуры мотора
// TODO: Добавить сохранение данных в EEPROM



void setup() {
  Serial.begin(9600);

  pinMode(BUTT_PIN, INPUT_PULLUP);

  light.begin();
  
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
  motor.setPower(got_data[0]);

  if (digitalRead(A0) == 0) {
    is_setting = true;
  }
}



void loop() {
  blt.update();
  light.update();
  motor.update();

  if (!is_setting) {
    if(blt.isConnect()) {
      motor.setMode(blt.getMotorMode());
      motor.setPower(blt.getMotorPower());

      light.setOn(blt.isLightOn());
      light.setBrightness(blt.getLightBright());
      light.setEffectByIndex(blt.getLightMode());
      light.setEffectColor(blt.getLightColor());
      light.setLightsBlink(blt.isLightBlink());
      light.setEffectSpeed(blt.getLightSpeed());
    }

    else {
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
        light.setOn(got_data[2]);

        send_data[0] = 20; // Отправляем данные о заряде
        send_data[1] = motor.getTemp(); // Отправляем данные о температуре
      }

      if (millis() - radio_timer > 5000) { // Если данные от пульта не поступали больше 5 сикунд
        // Выключаем мотор
        motor.setMode(Motor::mOff);
        motor.setPower(0);
        radio_timer = millis();
      }
    } 
  }

  // Режим настроек
  else { // В режиме настроек включаем спорт режим
    motor.setMode(Motor::mSport); // Максимальная чувствительность
    if (digitalRead(BUTT_PIN) == 0)
      motor.setPower(255);
    else
      motor.setPower(0);
  }
}