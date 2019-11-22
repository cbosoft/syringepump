typedef double (*setter_func)(long time);

double sine(long time);
double ramp(long time);
double constant(long time);
double get_setpoint(long time);

// vim: ft=arduino
