#include <Arduino.h>
#include "log.h"

void logToSerial(long load_cell_reading, unsigned long position, double speed) {
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
  Serial.print(speed, 10);
  Serial.print("\n");
}

// vim: ft=arduino
