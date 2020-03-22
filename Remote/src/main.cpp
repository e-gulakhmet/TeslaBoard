#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <GyverButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "remote.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
RF24 radio(RADIO_CS_PIN, RADIO_DO_PIN);
GButton button(BUTT_PIN, HIGH_PULL);

byte send_data[3];
byte got_data[3];
uint8_t power;
uint8_t mode;
String mode_name_[4] = {"Off", "Comfort", "Normal", "Sport"};



void showDisp() {
  static unsigned long disp_timer;

  if (millis() - disp_timer < 1000)
    return;
  
  disp_timer = millis();
  display.setTextSize(3);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,0);
  display.print(power);
  display.display();
}



void setup() {
  Serial.begin(9600);

  pinMode(POTENT_PIN, INPUT);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(8);     //размер пакета, в байтах

  radio.openWritingPipe(0xF0F0F0F0E1LL);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("WhoMan");
  display.display();
}


void loop() {
  button.tick();

  send_data[0] = map(analogRead(A1), 0, 1023, 0, 255); // Данные о положении потенциометра
  send_data[1] = button.isDouble(); // Двойное нажатие кнопки
  send_data[2] = button.isHolded(); // Если кнопка была нажата более 1 секунды

  radio.write(&send_data, 3);

  if (radio.isAckPayloadAvailable()) { // Если в буфере имеются принятые данные из пакета подтверждения приёма, то
    radio.read(&got_data, sizeof(got_data)); // Читаем данные из буфера в массив got_data указывая сколько всего байт может поместиться в массив.
    Serial.println("got");
    power = got_data[0];
    mode = got_data[1]; 
    showDisp();
  }





}