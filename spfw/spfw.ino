#include "motor.h"
#include "control.h"
#include "ruler.h"
#include "loadcell.h"
#include "optenc.h"
#include "log.h"



extern volatile unsigned long timer0_millis;

long start_millis = 0;
long time = 0;
double speed = 0.0; // mm/s
double flowrate = 0.0; // ml/s
double load_cell_reading = 0.0; // N
unsigned long position_nounits = 0;
double position = 0.0; // mm to end
double diameter = 21.5; // mm
double area = diameter * diameter * 0.25 * 3.1415926;

double control_action = 0.0;
void (*softReset)(void) = 0;




double calculateFlowrate(double speed)
{
  return speed * area * 0.001; // in ml/s
}




void setup () 
{

  Serial.begin(9600);
  Serial.print("ON\n");

  loadCellInit();
  motorInit();
  optencInit();
  controlInit();
  
  delay(500);

  Serial.print("START\n");

  delay(100);

  logTitlesToSerial();

  diameter = getInternalDiameterUnits();
  area = diameter * diameter * 0.25 * 3.1415926;

}




void loop ()
{
  time = millis() - start_millis;

  position_nounits = getPositionReading();
  position = convertPositionReadingUnits(position_nounits);
  
  load_cell_reading = getLoadCellReadingUnits();

  speed = getSpeedReading();
  flowrate = calculateFlowrate(speed);
  control_action = getControlAction(control_action, flowrate);
  motorSetDC(int(control_action));

  double current_diameter = getInternalDiameterUnits();

  if ((current_diameter < (diameter - 1.0)) || (current_diameter > (diameter + 1.0)) ) {
    Serial.print("STOP");
    softReset();
  }

  logToSerial(time, load_cell_reading, position, flowrate);

  if (getPositionReading() > RULER_POSITION_END) {

    for (int i = 0; i < 6; i++) {
      motorSetDC(0);
      Serial.print("STOP\n");
      delay(500);
    }

    // after a few seconds of repeating "STOP", reset
    softReset();
  }

  if (Serial.available())
    softReset();
}
