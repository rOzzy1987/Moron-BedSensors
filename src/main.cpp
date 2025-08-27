#include <Arduino.h>
#include <EEPROM.h>
#include <HX711.h>
#include "sensor_arr.h"
#include "menu.h"

#define LC_L_D   A4
#define LC_L_CLK A5
#define LC_R_D   A0
#define LC_R_CLK A1
#define LC_B_D   A2
#define LC_B_CLK A3
#define OUTPUT_PIN A7

HX711 loadCell0, loadCell1, loadCell2;
HX711 loadCells[] = {loadCell0, loadCell1, loadCell2};
SensorArray sensor;
Menu menu;

void setup() {
  loadCell0.begin(LC_L_D,LC_L_CLK);
  loadCell1.begin(LC_R_D,LC_R_CLK);
  loadCell2.begin(LC_B_D,LC_B_CLK);

  Serial.begin(57600);
  Serial.print("------------\nMoron multi load cell bed sensor\nv1.0");
  Serial.print("\n------------\n");
  Serial.print("(press <ESC> or type '?' for menu)");

  EEPROM.begin();
  sensor.begin(loadCells, 3);
  menu.begin(&sensor);
}

void loop() {
  // Serial.println(loadCell0.get_value());

  sensor.loop();
  menu.loop();

  digitalWrite(LED_BUILTIN, sensor.isTriggered());
  digitalWrite(OUTPUT_PIN, sensor.isTriggered());
}
