#include <Arduino.h>
#include <EEPROM.h>
#include <HX711.h>

#define MRNB_VER 1
HX711 lcL, lcR, lcB;
float sL, sR, sB;
uint32_t oL, oR, oB;
uint16_t sS, thr; 
bool isDebug = true;
unsigned long long prev;

void initCfg();
void loadCfg();
void saveCfg();

void setup() {
  lcL.begin(A0,A1);
  lcR.begin(A2,A3);
  lcB.begin(A4,A5);

  Serial.begin(9600);
  Serial.print("------------\nMoron triple load cell bed sensor\nv1.0.");
  Serial.print(MRNB_VER);
  Serial.print("n------------\n");

  delay(10000);
  if(Serial.available() == 0){
    Serial.end();
    isDebug = false;
  }

  EEPROM.begin();
  loadCfg();
}

void initCfg(){
  EEPROM.put(0, 'M');
  EEPROM.put(0, 'r');
  EEPROM.put(0, 'n');
  EEPROM.put(0, 'B');
  EEPROM.put(4, MRNB_VER);

  EEPROM.put(8, 1.0f);
  EEPROM.put(12, 1.0f);
  EEPROM.put(16, 1.0f);

  EEPROM.put(20, 0.0f);
  EEPROM.put(24, 0.0f);
  EEPROM.put(28, 0.0f);

  EEPROM.put(32, 10);
  EEPROM.put(34, 2000);
}

void loadCfg(){
  char hdr[] = {0,0,0,0, 0};
  hdr[0] = EEPROM.read(0);
  hdr[1] = EEPROM.read(1);
  hdr[2] = EEPROM.read(2);
  hdr[3] = EEPROM.read(3);
  if (strcmp("MrnB",hdr) != 0 || EEPROM.read(4) != MRNB_VER){
    if (isDebug) {
      Serial.print("Configuration header mismatch. Initializing defaults.\n");
    }
    initCfg();
  }

  EEPROM.get(8, sL);
  EEPROM.get(12, sR);
  EEPROM.get(16, sB);
  EEPROM.get(20, oL);
  EEPROM.get(24, oR);
  EEPROM.get(28, oB);

  EEPROM.get(32, sS);
  EEPROM.get(34, thr);

  
  if (isDebug) {
    Serial.print("Configuration loaded:\n");
    
    Serial.print("\nscale left:    ");
    Serial.print(sL);
    Serial.print("\nscale right:   ");
    Serial.print(sR);
    Serial.print("\nscale back:    ");
    Serial.print(sB);
    Serial.println();
    
    Serial.print("\noffset left:   ");
    Serial.print(oL);
    Serial.print("\noffset right:  ");
    Serial.print(oR);
    Serial.print("\noffset back:   ");
    Serial.print(oB);
    Serial.println();

    Serial.print("\nsample size:   ");
    Serial.print(sS);
    Serial.print("\nthreshold:     ");
    Serial.print(thr);
    Serial.println();
  }

  lcL.set_scale(sL);
  lcL.set_offset(oL);

  lcR.set_scale(sR);
  lcR.set_offset(oR);

  lcB.set_scale(sB);
  lcB.set_offset(oB);
  
}

void saveCfg() {
  

  EEPROM.put(8, sL);
  EEPROM.put(12, sR);
  EEPROM.put(16, sB);

  EEPROM.put(20, oL);
  EEPROM.put(24, oR);
  EEPROM.put(28, oB);

  EEPROM.put(32, sS);
  EEPROM.put(34, thr);
}

void loop() {

}
