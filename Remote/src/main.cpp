#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <GyverButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RF24 radio(9, 10);
GButton button(2, HIGH_PULL);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

byte send_data[4];
byte got_data[4];
uint8_t power;



void showDisp() {
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(power);
  display.display();
}



void setup() {
  Serial.begin(9600);

  pinMode(A1, INPUT);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(8);     //размер пакета, в байтах

  radio.openWritingPipe(pipes[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.clearDisplay();
  display.display();
}



void loop() {
  send_data[0] = map(analogRead(A1), 0, 1023, 0, 255); // Данные о положении потенциометра
  send_data[1] = button.isDouble(); // Двойное нажатие кнопки
  send_data[2] = button.isHolded(); // Если кнопка была нажата более 1 секунды

  radio.write(&send_data, 4);

  // if (radio.isAckPayloadAvailable()) { // Если в буфере имеются принятые данные из пакета подтверждения приёма, то ...
  //   radio.read(&got_data, sizeof(got_data)); // Читаем данные из буфера в массив got_data указывая сколько всего байт может поместиться в массив.
    
  //   power = got_data[0];
  //}

  button.tick();

  showDisp();
}