#include <bluetooth.h>



Bluetooth::Bluetooth(uint8_t rx_pin, uint8_t tx_pin)
    : rx_pin_(rx_pin)
    , tx_pin_(tx_pin)
    , blSerial_(rx_pin_, tx_pin_)
    {
        blSerial_.begin(9600);
    }



void Bluetooth::update() {
    parsing();

    if (is_connect_) {
      sett_mode_ = static_cast<SettingMode>(bl_data_[0]);
      switch (sett_mode_) {
        case smMain: {
          motor_data_.mode = bl_data_[1];
          int value = map(bl_data_[2], 20, 480, 0, 255);
          value = constrain(value, 0, 255);
          motor_data_.power = value;
          break;
        }
        
        case smLight: {
          light_data_.is_on = bl_data_[1];
          light_data_.mode = bl_data_[2];
          light_data_.bright = bl_data_[3];
          switch (light_data_.mode) {
            case 0: {
              light_data_.color = bl_data_[4];
              break;
            }

            case 1: {
              light_data_.is_blink = bl_data_[4];
              break;
            }

            case 2: {
              break;
            }
          }
          break;
        }
      }
    }
}



void Bluetooth::parsing() {
    static uint8_t index;
    static String string_convert = "";
    static boolean getStarted;
    if (blSerial_.available() > 0) {
        char incomingByte = blSerial_.read();        // обязательно ЧИТАЕМ входящий символ
        if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
        if (incomingByte != ' ' && incomingByte != ';') {   // если это не пробел И не конец
            string_convert += incomingByte;       // складываем в строку
        } else {                                // если это пробел или ; конец пакета
            bl_data_[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
            string_convert = "";                 // очищаем строку
            index++;                              // переходим к парсингу следующего элемента массива
        }
        }
        if (incomingByte == '$') {                // если это $
        getStarted = true;                      // поднимаем флаг, что можно парсить
        index = 0;                              // сбрасываем индекс
        string_convert = "";                    // очищаем строку
        }
        if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
        getStarted = false;                     // сброс
        is_connect_ = true;                    // флаг на принятие
        }
    }
}