#ifndef __LOG__
#define __LOG__

void logToSerial(long time, double load_cell_reading, double position, double speed);
void logTitlesToSerial();

#endif

// vim: ft=arduino
