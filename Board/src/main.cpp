#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <FastLED.h>

#include "motor.h"
#include "main.h"

RF24 radio(RADIO_CS_PIN, RADIO_DO_PIN);
Motor motor(MOTOR_PIN, TEMP_PIN);
CRGB leds[NUM_LEDS];

LightsMode lights_mode = emOneColor;

byte got_data[3];
byte send_data[3];
bool is_light = true;
bool is_setting;
unsigned long send_timer;
unsigned long radio_timer;
uint8_t idex;
uint8_t thishue = 0;
uint8_t thissat = 255;


// TODO: Дабавить датчик температуры


boolean safeDelay(int delTime) {
  static bool change_flag;
  uint32_t thisTime = millis();
  while (millis() - thisTime <= delTime) {
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

  FastLED.addLeds<WS2812B, LEDS_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50);
  
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
      is_light = got_data[2];

      send_data[0] = 20; // Отправляем данные о заряде
      send_data[1] = 30; // Отправляем данные о температуре
    }

    if (millis() - radio_timer > 5000) { // Если данные от пульта не поступали больше 5 сикунд
      // Выключаем мотор
      motor.setMode(Motor::mOff);
      motor.setPower(0);
      radio_timer = millis();
    }
    if (is_light) {
      switch (lights_mode) {
        case emOneColor: {
          fill_solid(&(leds[0]), NUM_LEDS, CRGB::Purple);
          break;
        }

        case emLights: {
          fill_solid(&(leds[0]), NUM_LEDS, CRGB::White);
          break;
        } 
        
        case emPolice: {
          idex++;
          if (idex >= NUM_LEDS) {
            idex = 0;
          }
          int idexR = idex;
          int idexB = antipodal_index(idexR);
          int thathue = (thishue + 160) % 255;
          for (int i = 0; i < NUM_LEDS; i++ ) {
            if (i == idexR) {
              leds[i] = CHSV(thishue, thissat, 255);
            }
            else if (i == idexB) {
              leds[i] = CHSV(thathue, thissat, 255);
            }
            else {
              leds[i] = CHSV(0, 0, 0);
            }
          }
          FastLED.show();
          if (safeDelay(10)) return;
          break;
        }

        case emPoliceStrobe: {
          idex++;
          if (idex >= NUM_LEDS) {
            idex = 0;
          }
          int idexR = idex;
          int idexB = antipodal_index(idexR);
          int thathue = (thishue + 160) % 255;
          leds[idexR] = CHSV(thishue, thissat, 255);
          leds[idexB] = CHSV(thathue, thissat, 255);
          FastLED.show();
          if (safeDelay(10)) return;
          break;
        }
        
        case emRainbow: {
          static uint8_t ihue;
          ihue++;
          if (ihue > 255) {
            ihue = 0;
          }
          for (int idex = 0 ; idex < NUM_LEDS; idex++ ) {
            leds[idex] = CHSV(ihue, thissat, 255);
          }
          FastLED.show();
          if (safeDelay(10)) return;
          break;
        }
      }
    }
    else {
      // Выключить подсветку
      FastLED.clear();
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

  FastLED.show();
  motor.update();
}