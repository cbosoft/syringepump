#include "hx711.h"

#include "loadcell.h"

HX711 loadcell;
const long LC_OFFSET = 0;
const long LC_DIVIDER = 1;



long getLoadCellReading()
{
  return loadcell.get_value(1);
}



double getLoadCellReadingUnits(long bytes)
{
  return (STRESS_M * bytes) + STRESS_C;
}




void loadCellInit()
{
  loadcell.begin(LC_DOUT_PIN, LC_CLK_PIN);
  loadcell.set_scale(LC_DIVIDER);
  loadcell.set_offset(LC_OFFSET);
}

// vim: ft=arduino
