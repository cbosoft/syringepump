#include <Arduino.h>
#include "optenc.h"

static volatile unsigned long tripc = 0;
static unsigned long time_last_check = 0;




void optMark()
{
  tripc ++;
}




unsigned long getTripCount()
{
  return tripc;
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
  return (double(dtrips) * TRIPS_PER_MM * MS_PER_S) / double(dt);
}




void optencInit()
{
  pinMode(OPT_PIN, INPUT);
  digitalWrite(OPT_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);
}





// vim: ft=arduino
