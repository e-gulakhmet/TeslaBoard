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
Light leds(LEDS_PIN, NUM_LEDS);
Bluetooth blt(BL_RX, BL_TX);

LightsMode lights_mode = emOneColor;

byte got_data[3];
byte send_data[3];
bool is_light;
bool is_setting;
unsigned long send_timer;
unsigned long radio_timer;
uint8_t idex;
uint8_t thishue = 0;
uint8_t thissat = 255;
uint8_t light_color = 0;
bool is_light_blink;



boolean safeDelay(int del_time) {
  static bool change_flag;
  uint32_t this_time = millis();
  while (millis() - this_time <= del_time) {
    if (change_flag) {
      change_flag = false;
      return true;
    }
  }
  return false;
}

int antipodal_index(int i) {
  int iN = i + NUM_LEDS / 2;
  if (i >= NUM_LEDS / 2) {
    iN = (i + NUM_LEDS / 2) % NUM_LEDS;
  }
  return iN;
}



void setup() {
  Serial.begin(9600);

  pinMode(BUTT_PIN, INPUT_PULLUP);

  leds.begin();
  
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
  leds.update();
  motor.update();

  leds.rainbow();

  if (!is_setting) {
    if(blt.isConnect()) {
      motor.setMode(blt.getMotorMode());
      motor.setPower(blt.getMotorPower());

      is_light = blt.isLightOn();
      lights_mode = static_cast<LightsMode>(blt.getLightMode());
      light_color = blt.getLightColor();
      is_light_blink = blt.isLightBlink();
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
        is_light = got_data[2];

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