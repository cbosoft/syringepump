#include <Arduino.h>
#include "hx711.h"

HX711 loadcell;

void setup()
{
  loadcell.begin(6, 7);
  loadcell.set_scale(1);
  loadcell.set_offset(0);
  Serial.begin(9600);
}

void loop()
{
  unsigned long val = loadcell.get_value(1);
  Serial.print(val);
  Serial.print("\n");
  delay(1000);
}


