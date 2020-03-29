#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <GyverButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "main.h"
#include "power.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
RF24 radio(RADIO_CS_PIN, RADIO_DO_PIN);
GButton button(BUTT_PIN, HIGH_PULL);
Power battery(POWER_PIN, BATTERY_PIN);

MotorMode motor_mode = mmComfort;

byte send_data[3];
byte got_data[3];
bool is_display = true;
uint8_t power;
String mode_name[4] = {"Off", "Eco", "Normal", "Sport"};
uint8_t board_battery;
uint8_t board_temp;
unsigned long connect_timer;
unsigned long battery_timer;
unsigned long display_timer;
bool is_connect;
bool is_lights;



MotorMode switchMotorMode(MotorMode mode, bool clockwise) { // Переключение режимов
  int n = static_cast<int>(mode);

  n += clockwise ? 1 : -1; // Если по часовой стрелке, то ставим следующий

  if ( n > 3) {
      n = 1;
  }
  if ( n < 1 ) {
      n = 1;
  }

  return static_cast<MotorMode>(n);
}



void showDisp() {
  static unsigned long disp_timer;

  if (millis() - disp_timer < 2000)
    return;

  disp_timer = millis();
  // Обновляем экран два раза в секунду.
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);

  // Отображаем информацию о пульте
  // Рисуем батарею
  display.drawRect(0, 0, 20, 10, HIGH); display.fillRect(20, 2, 2, 6, HIGH);
  // Значения на батареи
  display.fillRect(1, 1, map(battery.getProcent(), 0, 100, 0, 18), 8, HIGH);
  // Вольтаж батареи
  display.setCursor(26, 0);
  display.print(battery.getVoltage()); display.setCursor(50, 0); display.print("V");
  // Данные о подключении к доске
  if (is_connect)
    display.fillCircle(120, 5, 5, HIGH);
  else
    display.drawCircle(120, 5, 5, HIGH);

  // Отображаем данные о доске
  display.drawRect(0, 16, 128, 48, HIGH);
  display.setCursor(20, 18);
  display.print("BOARD");
  // Данные о положении курка газа
  display.setCursor(2, 30);
  display.print("POWER:"); display.setCursor(40, 30); display.print(power);
  // Данные о режиме работы доски
  display.setCursor(2, 42);
  display.print("MODE:"); display.setCursor(36, 42); display.print(mode_name[motor_mode]);
  // Данные о заряде батареи доски  
  display.setCursor(2, 54);
  display.print("BATT:"); display.setCursor(40, 54); display.print(board_battery);
  // Данные о температуре доски
  display.setCursor(68, 30);
  display.print("TEMP:"); display.setCursor(100, 30); display.print(board_temp);
  display.drawCircle(114, 32, 1, HIGH);
  // Данные о работе подсветки
  display.setCursor(68, 42);
  display.print("LGHT:"); display.setCursor(100, 42); display.print(is_lights);
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
  display.setTextColor(WHITE, BLACK);
  display.setCursor(20,0);
  display.print("Tesla");
  display.setCursor(20, 30);
  display.print("Board");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}


void loop() {
  button.tick();
  if (millis() - battery_timer > 60000) {
    battery_timer = millis();
    battery.update();
  }

  if (is_display && millis() - display_timer > 30000) {
    is_display = false;
    display.clearDisplay();
    display.display();
  }

  // Подготавливаем данные для отправки
  send_data[0] = power; // Данные о положении потенциометра
  send_data[1] = motor_mode; // Двойное нажатие кнопки
  send_data[2] = is_lights;
  // Отправеляем данные
  radio.write(&send_data, 3);
  
  // Получаем данные от приемника
   if (radio.isAckPayloadAvailable()) { // Если в буфере имеются принятые данные из пакета подтверждения приёма, то
    radio.read(&got_data, sizeof(got_data)); // Читаем данные из буфера в массив got_data указывая сколько всего байт может поместиться в массив.
    Serial.println("got");
    connect_timer = millis();
    is_connect = true;
    board_battery = got_data[0];
    board_temp = got_data[1];
  }

  power = map(analogRead(A1), 0, 1023, 0, 255); // Данные о положении потенциометра

  if (button.isClick()) { // Если кнопка была нажата два раза
    motor_mode = switchMotorMode(motor_mode, true); // Выбираем следущий режим
    display_timer = millis();
    is_display = true;
  }
  if (button.isHold()) { // Если было долгое нажатие на кнопку
    motor_mode = mmOff; // Включаем режим настроек
  }
  if (button.isDouble()) {
    is_lights = !is_lights;
  }

  if (millis() - connect_timer > 4000 && is_connect) {
    is_connect = false;
  }


  // Выводим информацию на димплей
  if (is_display) {
    showDisp();
  }
}