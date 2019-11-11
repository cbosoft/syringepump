#ifndef __LOADCELL__
#define __LOADCELL__

#define LC_DOUT_PIN 6
#define LC_CLK_PIN 7

//#define STRESS_M 2.47384526e-05
//#define STRESS_C -6.05960574e+00

// UPDATE NOVEMBER 8th
#define STRESS_M 2.91247522e-05
#define STRESS_C 4.48866159e-02

long getLoadCellReading();
double getLoadCellReadingUnits();
void loadCellInit();

#endif
