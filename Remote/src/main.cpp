#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <GyverButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "remote.h"
#include "object.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
RF24 radio(RADIO_CS_PIN, RADIO_DO_PIN);
GButton button(BUTT_PIN, HIGH_PULL);

MotorMode motor_mode = mmComfort;

byte send_data[3];
byte got_data[3];
uint8_t power;
String mode_name[4] = {"Off", "Eco", "Norm", "Sport"};
uint8_t battery_proc;
uint8_t motor_temp = 10;
int disp_update_period[4] = {10000, 2000, 3000, 5000};
unsigned long connect_timer;
bool is_connect;



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
  // Обновляем экран два раза в секунду.
  if (millis() - disp_timer < disp_update_period[motor_mode])
    return;
  
  disp_timer = millis();
  display.setTextColor(WHITE, BLACK);

  // Отображаем информацию о двигателе
  display.drawRect(0, 16, 128, 44, WHITE);
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.print("POWER: ");
  if (power < 100 && power >= 10) {
    display.print(power);
    display.print(" ");
  }
  else if (power < 10) {
    display.print(power);
    display.print("  ");
  }
  else {
    display.print(power);
  }
  display.setCursor(5, 40);
  display.print("MODE:");
  display.setCursor(65, 40);
  display.print(mode_name[motor_mode]);

  // Отображаем доп данные
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("BATT: "); 
  display.setCursor(30, 0);
  display.print(battery_proc); display.print("%");
  display.setCursor(55, 0);
  display.print("MOT_TEMP:");
  display.setCursor(110, 0);
  display.print(motor_temp);
  display.drawCircle(123, 1, 1, WHITE);
  display.fillRect(20, 62, 80, 2, is_connect);

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

  power = map(analogRead(A1), 0, 1023, 0, 255); // Данные о положении потенциометра

  if (button.isDouble()) { // Если кнопка была нажата два раза
    motor_mode = switchMotorMode(motor_mode, true); // Выбираем следущий режим
  }
  if (button.isHolded()) { // Если было долгое нажатие на кнопку
    motor_mode = mmOff; // Включаем режим настроек
  }

  if (millis() - connect_timer > 5000 && is_connect) {
    is_connect = false;
  }
  
  // Подготавливаем данные для отправки
  send_data[0] = power; // Данные о положении потенциометра
  send_data[1] = motor_mode; // Двойное нажатие кнопки
  // Отправеляем данные
  radio.write(&send_data, 3);

  // Получаем данные от приемника
  if (radio.isAckPayloadAvailable()) { // Если в буфере имеются принятые данные из пакета подтверждения приёма, то
    radio.read(&got_data, sizeof(got_data)); // Читаем данные из буфера в массив got_data указывая сколько всего байт может поместиться в массив.
    Serial.println("got");
    connect_timer = millis();
    is_connect = true;
    battery_proc = got_data[0];
    motor_temp = got_data[1];
  }

  // Выводим информацию на димплей
  showDisp();









}