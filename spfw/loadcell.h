#ifndef __LOADCELL__
#define __LOADCELL__

#define LC_DOUT_PIN 6
#define LC_CLK_PIN 7

#define STRESS_M 1.0
#define STRESS_C 0.0

long getLoadCellReading();
void loadCellInit();

#endif
