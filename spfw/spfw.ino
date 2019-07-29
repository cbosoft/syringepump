#include "motor.h"
#include "control.h"
#include "ruler.h"
#include "loadcell.h"
#include "optenc.h"
#include "log.h"



extern volatile unsigned long timer0_millis;
bool STOPPED = false;
bool REVERSING = false;
unsigned long position = 0;
double speed = 0.0;
int control_action;
long load_cell_reading = 0;




void setup () 
{

  Serial.begin(9600);
  Serial.print("ON\n");

  loadCellInit();
  motorInit();
  optencInit();
  controlInit();
  
  delay(500);

  // reset millis counter to zero
  noInterrupts();
  timer0_millis = 0;
  interrupts();

  Serial.print("START\n");

}



void loop ()
{
  position = getTripCount();//getPositionReading();
  load_cell_reading = getLoadCellReading();

  speed = 0;//getSpeedReading();
  control_action = getControlAction(speed);
  motorSetDC(control_action);

  logToSerial(load_cell_reading, position, speed);

  while (getPositionReading() > RULER_POSITION_END) {
    motorSetDC(0);
    Serial.print("STOP\n");
    delay(1000);
  }
}
