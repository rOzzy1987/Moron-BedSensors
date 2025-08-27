#ifndef __SENSOR_ARR_H__
#define __SENSOR_ARR_H__

#include <HX711.h>
#include <Arduino.h>
#include <EEPROM.h>

#define DRIFT_COMPENSATION_FACTOR 0.02
#define TRIGGER_TRESHOLD 100
#define SAMPLE_SIZE 1
#define EEPROM_FLAG 0xCA6F

#define DEBUG_TRIGGER 1
#define DEBUG_READINGS 2
#define DEBUG_SETTINGS 4
#define DEBUG_TIMING 8

class SensorArray {
    private:
        HX711 *_loadCells;
        uint8_t _count;
        long *_tares;
        float *_scales; 
        uint8_t _sampleSize = SAMPLE_SIZE;
        float *_readings;
        uint8_t _isTriggered;
        void trigger(uint8_t t);
        void setLoadCellSettings();
        // void getLoadCellSettings();
    public:
        uint8_t _debugFlags = 0;
        float _triggerTreshold = TRIGGER_TRESHOLD;
        float _driftCompFactor = DRIFT_COMPENSATION_FACTOR;
        uint8_t isTriggered() { return _isTriggered; }
        uint8_t getCount() { return _count; }

        void begin(HX711* loadCells, uint8_t loadCellCount){
            _loadCells = loadCells;
            _count = loadCellCount;
            _tares = new long[_count];
            _scales = new float[_count];
            _readings = new float[_count];
            loadSettings();
        }

        void loop();
        void tare();
        void setReading(float value, uint8_t idx);
        void saveSettings();
        void loadSettings();
        void initSettings();
        void debugSettings(const char* hdr = NULL);
};

#endif //__SENSOR_ARR_H__
