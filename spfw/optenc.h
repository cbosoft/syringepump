#ifndef __OPTENC__
#define __OPTENC__

#define OPT_PIN 2
#define TIME_LEN 10
#define MM_PER_TRIP 0.00272
#define MS_PER_S 1000.0

unsigned long getTripCount();
double getSpeedReading();
void optMark();
void optencInit();

#endif
