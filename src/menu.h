#ifndef __MENU_H__
#define __MENU_H__
#include <Arduino.h>
#include "sensor_arr.h"

#define MENU_TIMEOUT 10000 

class Menu {
    private:
        int _choice;
        unsigned long long _t;
        SensorArray *_sensor;

        void ReadChoice(){
            _t = millis();
            while (!Serial.available() && millis() - _t < MENU_TIMEOUT){

            }
            if (Serial.available()) {
                _choice = Serial.read();
                // Serial.println(_choice, HEX);
            } else {
                _choice = -1;
            }
        }
        void MainMenu();
        void DebugMenu();
        void SettingMenu();
        void EepromMenu();
        void SetReading(uint8_t idx);

    public:
        void begin(SensorArray *sensor) {
            Serial.setTimeout(MENU_TIMEOUT);
            _sensor = sensor;
        }

        void loop(){
            if (Serial.available()){
                _choice = Serial.read();
                if (_choice == 0x1B || _choice == '?'){
                    MainMenu();
                }
            }
        }

};

#endif //__MENU_H__