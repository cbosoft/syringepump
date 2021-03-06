#include <Arduino.h>
#include "optenc.h"

static volatile unsigned long tripc = 0;
static unsigned long time_last_check = 0;
unsigned long total_ticks = 0;




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

  total_ticks += dtrips;
  return (double(dtrips) * MM_PER_TRIP * MS_PER_S) / (double(dt));
}




void optencInit()
{
  pinMode(OPT_PIN, INPUT);
  digitalWrite(OPT_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);
}





// vim: ft=arduino
