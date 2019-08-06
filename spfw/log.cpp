#include <Arduino.h>
#include "log.h"

void logToSerial(long time, double load_cell_reading, double position, double speed)
{
  // time since start
  Serial.print(time);
  Serial.print(",");

  // load cell
  Serial.print(load_cell_reading, 5);
  Serial.print(",");

  // position (ruler)
  Serial.print(position, 5);
  Serial.print(",");

  // speed
  Serial.print(speed, 5);
  Serial.print("\n");
}

void logTitlesToSerial()
{
  // time since start
  Serial.print("Time (ms),");

  // load cell
  Serial.print("Load Cell (N),");

  // position (ruler)
  Serial.print("Distance to end (mm),");

  // speed
  Serial.print("Flowrate (ml/s)\n");
}

// vim: ft=arduino
