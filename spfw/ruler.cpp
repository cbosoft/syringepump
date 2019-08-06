#include <Arduino.h>
#include "ruler.h"


const double diameters[10] = {5, 10, 20, 30, 40};


double getInternalDiameterUnits()
{
  // TODO

  return 1.0;
}




double getDiameterReadingUnits()
{
  return convertDiameterReadingUnits(getDiameterReading());
}




double convertDiameterReadingUnits(unsigned long diameter_nounits)
{
  double byte_dia_f = double(diameter_nounits);

  return (byte_dia_f * RULER_DIAMETER_CAL_M) + RULER_DIAMETER_CAL_C;
}




double getPositionReadingUnits()
{
  return convertPositionReadingUnits(getPositionReading());
}




double convertPositionReadingUnits(unsigned long position_nounits)
{
  double byte_pos_f = double(position_nounits);

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
