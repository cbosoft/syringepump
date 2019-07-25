#include <Arduino.h>
#include "optenc.h"

volatile long tripc = 0;
unsigned long time_last_check = 0;
const double trips_per_mm = 1000.0;
const double ms_per_s = 1000.0;




void optMark()
{
  tripc ++;
}




double getSpeedReading()
{
  noInterrupts();
  unsigned long now = millis();
  unsigned long dt = now - time_last_check;
  time_last_check = now;
  long dtrips = tripc;
  tripc = 0;
  interrupts();
  return (double(dtrips) * trips_per_mm * ms_per_s) / double(dt);
}



void optencInit()
{
  pinMode(OPT_PIN, INPUT);
  digitalWrite(OPT_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);
}

// vim: ft=arduino
