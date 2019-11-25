#include "Arduino.h"
#include "setter.h"

setter_func setter;

// setter properties
double sine_magnitude = 1.0;
double sine_mean = 0.0;
double sine_frequency = 1.0; // Hz

double constant_val = 120.0;

double ramp_gradient = 0.1;
double ramp_intercept = 0; // positive


double sine(long time)
{
  double dtime = ((double)time)*0.001;
  return (sin(dtime*PI*sine_frequency) * sine_magnitude) + sine_mean;
}

double ramp(long time)
{
  double dtime = ((double)time)*0.001;
  return (ramp_gradient*dtime) + ramp_intercept;
}

double constant(long time)
{
  return constant_val;
}

double get_setpoint(long time)
{
  return (*setter)(time);
}







// vim: ft=arduino
