#ifndef __RULER__
#define __RULER__

#define RULER_POSITION_PIN A0
#define RULER_POSITION_END 960
#define RULER_POSITION_START 0
#define RULER_POSITION_CAL_M -0.12646004
#define RULER_POSITION_CAL_C 124.12431658

#define RULER_DIAMETER_PIN A1
#define RULER_DIAMETER_CAL_M 1.0
#define RULER_DIAMETER_CAL_C 0.0

unsigned long getPositionReading();
unsigned long getDiameterReading();

double convertPositionReadingUnits(unsigned long position_nounits);
double convertDiameterReadingUnits(unsigned long diameter_nounits);
double getPositionReadingUnits();
double getDiameterReadingUnits();
double getInternalDiameterUnits();

#endif

// vim: ft=arduino
