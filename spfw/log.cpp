#include <Arduino.h>
#include "log.h"

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

  Serial.print("Load Cell (N),");

  Serial.print("Distance to end (mm),");

  Serial.print("Flowrate (ml/s)\n");
}

// vim: ft=arduino
