#include "Arduino.h"
#include "setter.h"

setter_func setter;

// setter properties
double sp_A = 0.0;
double sp_B = 0.0;
double sp_C = 0.0;

double square(long time)
{
  // A is freq in Hz, B and C are hi and lo values of wave
  double dtime = ((double)time)*0.001;
  double period_extent = dtime*sp_A;
  int hi_or_lo = ((int)period_extent) % 2;
  return (hi_or_lo) ? sp_B : sp_C;
}


double sine(long time)
{
  // A is freq, B is mag, C is mean
  double dtime = ((double)time)*0.001;
  return (sin(dtime*PI*sp_A) * sp_B) + sp_C;
}

double ramp(long time)
{
  // A is grad, B is intercept
  double dtime = ((double)time)*0.001;
  return (dtime * sp_A) + sp_B;
}

double step(long time)
{
  // A is initial, B is switch time, C is final
  double dtime = ((double)time)*0.001;
  return (dtime < sp_B) ? sp_A : sp_C;
}

double constant(long time)
{
  // A is const
  return sp_A;
}

double get_setpoint(long time)
{
  return (*setter)(time);
}







// vim: ft=arduino
