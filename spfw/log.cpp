#include <Arduino.h>
#include "log.h"

extern int raw_lc;

void logToSerial(long time, double load_cell_reading, double position, double flowRate)
{
  Serial.print(time);
  Serial.print(",");

  Serial.print(load_cell_reading, 5);
  Serial.print(",");

  Serial.print(position, 5);
  Serial.print(",");

  Serial.print(flowRate, 5);
  Serial.print("\n");
}

void logTitlesToSerial()
{
  Serial.print("Time (ms),");

  if (raw_lc)
    Serial.print("Load Cell (b)");
  else
    Serial.print("Load Cell (N),");

  Serial.print("Distance to end (mm),");

  Serial.print("Flowrate (ml/s)\n");
}

// vim: ft=arduino
