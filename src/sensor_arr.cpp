#include "sensor_arr.h"

static unsigned long long __t;

void SensorArray::setLoadCellSettings() {
    for(uint8_t i = 0; i < _count; i++) {
        _loadCells[i].set_offset(_tares[i]);
        _loadCells[i].set_scale(_scales[i]);
    }
}
void SensorArray::loop(){
    float value = 0;
    if (_debugFlags & DEBUG_TIMING){
        __t = millis();
    }

    for (uint8_t i = 0; i < _count; i++) {
        _readings[i] = _loadCells[i].get_units(_sampleSize);
        value += _readings[i];
    }

    if (_debugFlags & DEBUG_TIMING){
        __t = millis() - __t;
        Serial.print("Readins done in ");
        Serial.print((int)__t);
        Serial.print("ms");
        Serial.println();
    }


    if (_debugFlags & DEBUG_READINGS){
        for (uint8_t i = 0; i < _count; i++) {
            Serial.print(i);
            Serial.print(":");
            Serial.print((int)_readings[i]);
            Serial.print("\t");
        }
        Serial.print("Sum:");
        Serial.print(value);
        Serial.println();
    }

    trigger(value > _triggerTreshold);
    if (!_isTriggered) {
        if (value < 0) {
            // Negative weight, something doesn't add up or thermal drift
            tare();
        } else {
            // adjust towards 0 measurement 
            for (uint8_t i = 0; i < _count; i++) {
                _readings[i] = _loadCells[i].get_units(_sampleSize);
                _tares[i] += _readings[i] * _driftCompFactor;
            }
            setLoadCellSettings();
        }
    }
}
void SensorArray::trigger(uint8_t t){
    if ((_debugFlags & DEBUG_TRIGGER) && (_isTriggered != t)){
        Serial.println(t ? "Triggered!" : "Trigger unset!");
    }

    _isTriggered = t;
}
void SensorArray::tare(){
    for (uint8_t i = 0; i < _count; i++) {
        _tares[i] = _loadCells[i].read_average(_sampleSize*10);
    }
    setLoadCellSettings();
}
void SensorArray::setReading(float value, uint8_t idx){
    float reading = _loadCells[idx].get_value(_sampleSize * 10);
    _scales[idx] = reading / value;
    setLoadCellSettings();
}

void SensorArray::debugSettings(const char* hdr = NULL) {
        Serial.println(hdr == NULL ? "Settings:" : hdr);
        Serial.println("----------------");
        Serial.print("Trigger treshold:           ");
        Serial.println(_triggerTreshold);
        Serial.print("Drift compensation factor:  ");
        Serial.println(_driftCompFactor);
        Serial.print("Sample size:                ");
        Serial.println(_sampleSize);
        
        for (uint8_t i=0; i < _count; i++){
            Serial.print("Load cell #");
            Serial.print(i);
            Serial.print(" tare:      ");
            Serial.println(_tares[i]);
        }
        for (uint8_t i=0; i < _count; i++){
            Serial.print("Load cell #");
            Serial.print(i);
            Serial.print(" scale:     ");
            Serial.println(_scales[i]);
        }
}

void SensorArray::initSettings() {
    if (_debugFlags & DEBUG_SETTINGS) {
        Serial.println("Initializing clean settings");
    }
    _triggerTreshold = TRIGGER_TRESHOLD;
    _sampleSize = SAMPLE_SIZE;
    _driftCompFactor = DRIFT_COMPENSATION_FACTOR;
    for (uint8_t i = 0; i < _count; i++) {
        _tares[i] = 0;
        _scales[i] = 1;
    }
    saveSettings();
}
void SensorArray::saveSettings() {
    if (_debugFlags & DEBUG_SETTINGS) {
        Serial.println("Saving settings");
    }
    uint8_t addr = 0;
    uint16_t flag= EEPROM_FLAG;

    EEPROM.put(addr, flag);
    addr += sizeof(uint16_t);

    EEPROM.put(addr, _count);
    addr += sizeof(uint8_t);

    EEPROM.put(addr, _triggerTreshold);
    addr += sizeof(float);

    EEPROM.put(addr, _driftCompFactor);
    addr += sizeof(float);

    EEPROM.put(addr, _sampleSize);
    addr += sizeof(uint8_t);

    for (uint8_t i=0; i < _count; i++){
        EEPROM.put(addr, _tares[i]);
        addr += sizeof(long);
    }
    for (uint8_t i=0; i < _count; i++){
        EEPROM.put(addr, _scales[i]);
        addr += sizeof(float);
    }
}
void SensorArray::loadSettings() {
    if (_debugFlags & DEBUG_SETTINGS) {
        Serial.println("Loading Settings");
    }

    uint8_t addr = 0, count;
    uint16_t flag;
    EEPROM.get(addr, flag);
    addr += sizeof(uint16_t);
    EEPROM.get(addr, count);
    addr += sizeof(uint8_t);

    if (flag != EEPROM_FLAG || count != _count) {
        if (_debugFlags &DEBUG_SETTINGS) {
            Serial.print("Flag mismatch:");
            Serial.print(flag, HEX);
            Serial.print(" vs expected ");
            Serial.print(EEPROM_FLAG, HEX);
            Serial.println();
        }
        initSettings();
    } else {
        EEPROM.get(addr, _triggerTreshold);
        addr += sizeof(float);

        EEPROM.get(addr, _driftCompFactor);
        addr += sizeof(float);

        EEPROM.get(addr, _sampleSize);
        addr += sizeof(uint8_t);

        for (uint8_t i=0; i < _count; i++){
            EEPROM.get(addr, _tares[i]);
            addr += sizeof(long);
        }
        for (uint8_t i=0; i < _count; i++){
            EEPROM.get(addr, _scales[i]);
            addr += sizeof(float);
        }
    }
    if (_debugFlags & DEBUG_SETTINGS) {
        debugSettings("Loaded settings:");
    }
    setLoadCellSettings();
}