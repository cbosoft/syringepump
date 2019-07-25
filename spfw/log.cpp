#include <Arduino.h>
#include "log.h"

void logToSerial(long load_cell_reading, int position, double speed) {
  // time since start
  Serial.print(millis());
  Serial.print(",");
  
  // load cell
  Serial.print(load_cell_reading);
  Serial.print(",");
  
  // position (ruler)
  Serial.print(position);
  Serial.print(",");
  
  // speed
  Serial.print(speed);
  Serial.print("\n");
}

// vim: ft=arduino
