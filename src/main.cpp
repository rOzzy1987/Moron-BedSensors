#include <Arduino.h>
#include <EEPROM.h>
#include <HX711.h>

#define MRNB_VER 2

// Memory offsets
#define MO_LS 8
#define MO_RS 12
#define MO_BS 16
#define MO_LO 20
#define MO_RO 24
#define MO_BO 28
#define MO_SS 32
#define MO_THR 34

HX711 lcL, lcR, lcB;
float sL, sR, sB;
int32_t oL, oR, oB;
uint16_t sS, tt; 
bool isDebug = false;
bool writeValues = true;
unsigned long long prev;

char* buff = new char[32];
int8_t bptr = 0;



void printCfg(const char* hdr = "Configuration:"){
    Serial.print(hdr);
    Serial.print("\n");

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
    Serial.print(tt);
    Serial.println();
}

void saveCfg() {
  if(isDebug){
    Serial.println("Configuration saved!");
  }
  EEPROM.put(MO_LS, sL);
  EEPROM.put(MO_RS, sR);
  EEPROM.put(MO_BS, sB);

  EEPROM.put(MO_LO, oL);
  EEPROM.put(MO_RO, oR);
  EEPROM.put(MO_BO, oB);

  EEPROM.put(MO_SS, sS);
  EEPROM.put(MO_THR, tt);
}

void initCfg(){
  EEPROM.put(0, 'M');
  EEPROM.put(1, 'r');
  EEPROM.put(2, 'n');
  EEPROM.put(3, 'B');
  EEPROM.put(4, MRNB_VER);

  EEPROM.put(MO_LS, 1.0f);
  EEPROM.put(MO_RS, 1.0f);
  EEPROM.put(MO_BS, 1.0f);

  EEPROM.put(MO_LO, 0.0f);
  EEPROM.put(MO_RO, 0.0f);
  EEPROM.put(MO_BO, 0.0f);

  EEPROM.put(MO_SS, 1);
  EEPROM.put(MO_THR, 2000);
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

  EEPROM.get(MO_LS, sL);
  EEPROM.get(MO_RS, sR);
  EEPROM.get(MO_BS, sB);

  EEPROM.get(MO_LO, oL);
  EEPROM.get(MO_RO, oR);
  EEPROM.get(MO_BO, oB);

  EEPROM.get(MO_SS, sS);
  EEPROM.get(MO_THR, tt);

  
  if (isDebug) {
    printCfg("Configuration loaded:");
  }

  lcL.set_scale(sL);
  lcL.set_offset(oL);

  lcR.set_scale(sR);
  lcR.set_offset(oR);

  lcB.set_scale(sB);
  lcB.set_offset(oB);
}

void processCommand(){
  // Serial.print("- Command received: >");
  // Serial.print(buff);
  // Serial.print("<\n---------\n");

  if(strcmp("help", buff) == 0){
    Serial.print("Available commands:\n");
    Serial.print("val                    toggle writing values to serial\n");
    Serial.print("cfg                    display configuration values\n");
    Serial.print("tare                   tare all sensors\n");

    Serial.print("[lo|ro|bo] <OFFSET>    set left/right/back offset\n");
    Serial.print("                       used for taring the sensors\n");
    Serial.print("    <OFFSET>:          32 bit integer\n");
    Serial.print("[ls|rs|bs] <SCALE>     set left/right/back scale\n");
    Serial.print("                       the scale factor to convert raw values to weight\n");
    Serial.print("    <SCALE>:           float\n");
    Serial.print("ss <SAMPLES>           set sample size\n");
    Serial.print("                       samples to read for averaging\n");
    Serial.print("    <SAMPLES>:         16 bit integer\n");
    Serial.print("tt <THRESHOLD>         set time threshold\n");
    Serial.print("                       if measurement dos not change within the given time\n");
    Serial.print("                       the sensor will be re-tared\n");
    Serial.print("    <THRESHOLD>:       16 bit integer\n");

    Serial.print("bo 1234      set back offset to 1234\n");
    return;
  }
  else if(strcmp("val", buff) == 0) {
    writeValues = !writeValues;
    return;
  }
  else if(strcmp("cfg", buff) == 0) {
    printCfg();
    return;
  }
  else if(strcmp("tare", buff) == 0) {
    lcL.tare();
    lcR.tare();
    lcB.tare();
    oL = lcL.get_offset();
    oR = lcR.get_offset();
    oB = lcB.get_offset();
    saveCfg();
    return;
  }
  else if(strcmp("lo", buff) == 0) oL = Serial.parseInt();
  else if(strcmp("ro", buff) == 0) oR = Serial.parseInt();
  else if(strcmp("bo", buff) == 0) oB = Serial.parseInt();
  else if(strcmp("ls", buff) == 0) sL = Serial.parseFloat();
  else if(strcmp("rs", buff) == 0) sR = Serial.parseFloat();
  else if(strcmp("bs", buff) == 0) sB = Serial.parseFloat();
  else if(strcmp("ss", buff) == 0) sS = Serial.parseInt();
  else if(strcmp("tt", buff) == 0) tt = Serial.parseInt();
  else return;
  saveCfg();
  loadCfg();
}

void handleCommand(){
  bptr = 0;
  prev = millis();

  while (millis() - prev < 10000) {
    int c  = Serial.read();
    if (c == '\t') {
      writeValues = !writeValues;
      return;
    }
    if(c == -1) continue;
    
    buff[bptr++] = c;
    if (bptr > 0 && (buff[bptr - 1] == '\r' || buff[bptr - 1] == '\n' || buff[bptr -1] == ' ')){
      buff[bptr - 1] = 0;
      processCommand();
      return;
    }
    
  }
}

void setOutput(bool triggered){
  digitalWrite(LED_BUILTIN, triggered);
  digitalWrite(A7, triggered);
}

void setup() {
  lcL.begin(A2,A3);
  lcR.begin(A0,A1);
  lcB.begin(A4,A5);

  Serial.begin(57600);
  Serial.print("------------\nMoron triple load cell bed sensor\nv1.0.");
  Serial.print(MRNB_VER);
  Serial.print("\n------------\n");

  prev = millis();
  while (millis() - prev < 10000){
    if (Serial.available()) {
      isDebug = true;
      break;
    }
  }
  if(!isDebug){
    Serial.println("No input, disabling debug mode.");
    Serial.end();
  } else {
    Serial.readBytes(buff, 32);
    Serial.setTimeout(10000);
  }

  EEPROM.begin();
  loadCfg();
  prev=millis();
}

void loop() {
  float vL,vR,vB,vS;
  unsigned long long t0,t1,t2,t3;
  t0 = millis();
  vL = lcL.get_units(sS);
  t1 = millis();
  vR = lcR.get_units(sS);
  t2 = millis();
  vB = lcB.get_units(sS);
  t3 = millis();
  vS = vL + vR + vB;

  setOutput(abs(vS) > 25);

  if (isDebug){
    if(writeValues) {
      Serial.print((int)(t1-t0));
      Serial.print("\t");
      Serial.print((int)(t2-t0));
      Serial.print("\t");
      Serial.print((int)(t3-t0));
      Serial.print("\t");
      
      Serial.print(vL);
      Serial.print("\t");
      Serial.print(vR);
      Serial.print("\t");
      Serial.print(vB);
      
      Serial.print("\t");
      Serial.print(vS);

      Serial.println();
    }
    if(Serial.available()){
      handleCommand();
    }
  }
}
