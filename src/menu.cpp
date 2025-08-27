#include "menu.h"

void Menu::MainMenu(){
    _choice = 1;
    while (_choice != -1){
        Serial.println("\n\n Main menu\n---------------");
        Serial.println("1) Debug settings");
        Serial.println("2) Load cell settings");
        Serial.println("3) Setting & EEPROM handling");
        Serial.println("\nQ) Quit");

        ReadChoice();
        switch (_choice) {
            case 'q':
            case 'Q':
            case 0x1B:
                Serial.println("Quitting menu\n----------------");
                return;
            case '1':
                DebugMenu();
                break;
            case '2':
                SettingMenu();
                break;
            case '3':
                EepromMenu();
                break;
        }
    }
}

void Menu::DebugMenu(){

    _choice = 1;
    while (_choice != -1){
        Serial.println("\n\n Debug menu\n---------------");
        Serial.print("1) Display measurement time:   ");
        Serial.println(_sensor->_debugFlags & DEBUG_TIMING ? "on" : "off");
        Serial.print("2) Display measurements:       ");
        Serial.println(_sensor->_debugFlags & DEBUG_READINGS ? "on" : "off");
        Serial.print("3) Log setting events:         ");
        Serial.println(_sensor->_debugFlags & DEBUG_SETTINGS ? "on" : "off");
        Serial.print("4) Log trigger events:         ");
        Serial.println(_sensor->_debugFlags & DEBUG_TRIGGER ? "on" : "off");
        Serial.println("\nB) back");
        
        ReadChoice();
        switch (_choice) {
            case 'b':
            case 'B':
            case 0x1B:
                _choice = 'X';
                return;
            case '1':
                _sensor->_debugFlags ^= DEBUG_TIMING;
                break;
            case '2':
                _sensor->_debugFlags ^= DEBUG_READINGS;
                break;
            case '3':
                _sensor->_debugFlags ^= DEBUG_SETTINGS;
                break;
            case '4':
                _sensor->_debugFlags ^= DEBUG_TRIGGER;
                break;
            
        }
    }
    _choice = 'X';
}

void Menu::SettingMenu(){

    float value;
    _choice = 1;
    while (_choice != -1){
        Serial.println("\n\n Setting menu\n---------------");
        Serial.print("1) Set drift compensation factor:   ");
        Serial.println(_sensor->_driftCompFactor);
        Serial.print("2) Set trigger treshold:            ");
        Serial.println(_sensor->_triggerTreshold);
        Serial.println("3) Tare all sensors");
        for( uint8_t i = 0; i < _sensor->getCount(); i++) {
            Serial.print(i+4);
            Serial.print(") Set measured value for load cell #");
            Serial.println(i);
        }
        Serial.println("\nB) back");
        
        ReadChoice();
        switch (_choice) {
            case 'b':
            case 'B':
            case 0x1B:
                _choice = 'X';
                return;
            case '1':
                Serial.println("\nEnter desired value for drift compensation factor (press <space> when done):");
                Serial.print("Current value: "),
                Serial.println(_sensor->_driftCompFactor);

                value = Serial.parseFloat();
                if(value < 0.0f || value > 1.0f) {
                    Serial.println("\nValue must be between 0 and 1");
                } else {
                    _sensor->_driftCompFactor = value;
                }
                break;
            case '2':
                Serial.println("\nEnter desired value for trigger treshold (press <space> when done):");
                Serial.print("Current value: "),
                Serial.println(_sensor->_triggerTreshold);

                value = Serial.parseFloat();
                _sensor->_triggerTreshold = value;
                break;
            case '3':
                _sensor->tare();
                break;
            case '4':
                SetReading(0);
                break;
            case '5':
                SetReading(1);
                break;
            case '6':
                SetReading(2);
                break;
            case '7':
                SetReading(3);
                break;
        }
    }
    _choice = 'X';
}


void Menu::EepromMenu(){
    _choice = 1;
    while (_choice != -1){
        Serial.println("\n\n Eeprom menu\n---------------");
        Serial.println("1) Save Config to EEPROM");
        Serial.println("2) Reload config from EEPROM");
        Serial.println("3) Initialize config from EEPROM");
        Serial.println("4) Show current settings");
        Serial.println("\nB) back");
        
        ReadChoice();
        switch (_choice) {
            case 'b':
            case 'B':
            case 0x1B:
                _choice = 'X';
                return;
            case '1':
                _sensor->saveSettings();
                break;
            case '2':
                _sensor->loadSettings();
                break;
            case '3':
                _sensor->initSettings();
                break;
            case '4':
                _sensor->debugSettings();
                break;
        }
    }
    _choice = 'X';
}
void Menu::SetReading(uint8_t idx){
        if(idx >= _sensor->getCount()) return;

        Serial.print("\nPlace a known weight on load cell #");
        Serial.println(idx);
        Serial.println("Enter current weight (press <space> when done):");
        
        float value = Serial.parseFloat();
        if(value < 0.0f) {
            Serial.println("\nValue must be greater than 0");
        } else {
            _sensor->setReading(value, idx);
        }
}