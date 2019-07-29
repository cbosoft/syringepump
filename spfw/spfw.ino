#include "motor.h"
#include "control.h"
#include "ruler.h"
#include "loadcell.h"
#include "optenc.h"
#include "log.h"



extern volatile unsigned long timer0_millis;
unsigned long position = 0;
double speed = 0.0;
int control_action = 0;
long load_cell_reading = 0;
void (*softReset)(void) = 0;




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
  position = getPositionReading();
  load_cell_reading = getLoadCellReading();

  speed = getSpeedReading();
  control_action = getControlAction(speed);
  motorSetDC(control_action);

  logToSerial(load_cell_reading, position, speed);

  if (getPositionReading() > RULER_POSITION_END) {
    for (int i = 0; i < 6; i++) {
      motorSetDC(0);
      Serial.print("STOP\n");
      delay(500);
    }

    // after a few seconds of repeating "STOP", reset
    softReset();
  }
}
