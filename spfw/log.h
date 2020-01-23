#ifndef __LOG__
#define __LOG__

void logToSerial(long time, double flowrate, double force, double control_action, long load_cell_reading, unsigned long ticks);
void logTitlesToSerial();

#endif

// vim: ft=arduino
