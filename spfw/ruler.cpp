#include <Arduino.h>
#include "ruler.h"

double getPositionReadingMillimeter()
{
  double byte_pos_f = double(getPositionReading());

  return (byte_pos_f * RULER_POSITION_CAL_M) + RULER_POSITION_CAL_C;
}



unsigned long getPositionReading()
{
  return analogRead(RULER_POSITION_PIN);
}




unsigned long getDiameterReading()
{
  return analogRead(RULER_DIAMETER_PIN);
}

// vim: ft=arduino
