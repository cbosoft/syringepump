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
long load_cell = 0.0; // 24b
double force = 0; // N
unsigned long position_nounits = 0;
double position = 0.0; // mm to end
extern double buflen;
extern double inner_diameter;
extern int log_options;

double control_action = 0.0;
void (*softReset)(void) = 0;




double calculateFlowrate(double speed)
{
  return speed * inner_diameter * inner_diameter * 0.25 * 3.1415926 * 0.001; // in ml/s
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

}




void loop ()
{
  time = millis() - start_millis;

  position_nounits = getPositionReading();
  position = convertPositionReadingUnits(position_nounits);
  
  load_cell = getLoadCellReading();
  
  force = getLoadCellReadingUnits(load_cell);

  speed = getSpeedReading();
  flowrate = calculateFlowrate(speed);

  control_action = getControlAction(control_action, flowrate, force);
  motorSetDC(int(control_action));

  logToSerial(time, force, flowrate, control_action, load_cell, -1);

  if (position < buflen) {

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
