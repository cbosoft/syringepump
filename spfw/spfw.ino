#include <SPI.h>
#include "hx711.h"

// MOTOR {{{

// There are 6 pwm pins on the Arduino UNO: pins 3, 5, 6, 9, 10, and 11.
const int MOTOR_PWM_PIN = 4;

// Motor direction is controlled by H-bridge: two transistors control the
// direction. If PIN_1 is high and PIN_2 is low... TODO
const int MOTOR_ENABLE_PIN = 4;
const int MOTOR_DIR_PIN_1 = 5;
const int MOTOR_DIR_PIN_2 = 6;

#define REVERSE 0
#define FORWARD 1

// }}}
// CONTROL {{{

#define ERR_HIST_LEN 1000
static long speed_set_point = 0;
static long speed_current = 0;
static float err_hist[ERR_HIST_LEN] = {0};
static unsigned int err_count = 0;
static float kp = 0.2;
static float ki = 0.0;
static float kd = 0.0;

// }}}
// LOAD CELL {{{

HX711 loadcell;
const int LC_DOUT = 7;
const int LC_CLK = 8;
const long LC_OFFSET = 0;
const long LC_DIVIDER = 1;

// }}}
// RULERS {{{
const int RULER_POSITION_PIN = A0;
const int RULER_POSITION_END = 960;
int RULER_POSITION_START = 0;

const int RULER_DIAMETER_PIN = A1;
// }}}
// OPTICAL ENCODER {{{

// Arduino UNO has two hardware interrupt pins: 2, 3;
#define OPT_PIN 2
#define TIME_LEN 10
static unsigned long time_hist[TIME_LEN] = {0};
static unsigned int time_count = 0;
const float OPT_MASK_ARC_LEN = PI*2.0*0.25; // quarter turn
// }}}

static bool STOPPED = 0;
static bool REVERSING = 0;




void motorSetDirection(int dir)
{
  digitalWrite(MOTOR_DIR_PIN_1, dir);
  digitalWrite(MOTOR_DIR_PIN_2, !dir);
}

void motorSetDC(unsigned int dc)
{
  if (dc > 255) {
    dc = 255;
  }

  analogWrite(MOTOR_PWM_PIN, dc);

  digitalWrite(MOTOR_ENABLE_PIN, (dc == 0)?LOW:HIGH);
}


float getSpeed()
{
  // TODO
  // calculate speed

  // set speed_current
  //speed_current = XX;

  // then return value
  return speed_current;
}




long getLoadCellReading()
{
  return loadcell.get_value(1);
}




long getPositionReading()
{
  return analogRead(RULER_POSITION_PIN);
}




long getDiameterReading()
{
  return analogRead(RULER_DIAMETER_PIN);
}




void updateMotorDC()
{
  // TODO
  // Calculate CA based on past values of speed, set point
  // -> PID algorithm
  
  // normalise CA to range 0-255 -> DC
  // set CA (DC) on motor PWM
  motorSetDC(120);
}




void optMark()
{
  if (time_counts < TIME_LEN) {
    times[time_counts] = millis();
    time_counts ++;
  }
  else {
    for (int i = 1; i < TIME_LEN; i++) {
      times[i-1] = times[i];
    }
    times[TIME_LEN-1] = millis();
  }
}




void logToSerial() {
  // time since start
  Serial.print(millis());
  Serial.print(",");
  
  // load cell
  Serial.print(getLoadCellReading());
  Serial.print(",");
  
  // position (ruler)
  Serial.print(getPositionReading());
  Serial.print(",");
  
  // diameter (ruler)
  Serial.print(getDiameterReading());
  Serial.print(",");

  // speed
  Serial.print(speed_current);
  Serial.print("\n");
}



void checkPosition()
{
  long position = getPositionReading();
  
  if ((position > RULER_POSITION_END) && (!REVERSING)) {
    motorSetDirection(REVERSE);
    motorSetDC(255);
    REVERSING = true;
  }
  else if ((position < RULER_POSITION_START) && (REVERSING)) {
    motorSetDC(0);
    STOPPED = true;
  }
}




void setup () 
{
  Serial.begin(9600);

  // Load cell
  loadcell.begin(LC_DOUT, LC_CLK);
  loadcell.set_scale(LC_DIVIDER);
  loadcell.set_offset(LC_OFFSET);

  // Motor setup
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN_1, OUTPUT);
  pinMode(MOTOR_DIR_PIN_2, OUTPUT);
  motorSetDirection(1);
  motorSetDC(0);

  // Optical encoder setup
  pinMode(OPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);

  bool got_setpoint = false;
  bool got_kp = false;
  bool got_ki = false;
  bool got_kd = false;

  while (!got_setpoint || !got_kp || !got_ki || !got_kd) {
    Serial.print("Waiting for \n");
    if (!got_setpoint) Serial.print("  - set point\n");
    if (!got_kp) Serial.print("  - Kp\n");
    if (!got_ki) Serial.print("  - Ki\n");
    if (!got_kd) Serial.print("  - Kd\n");

    while (!Serial.available()) delay(100);

    char recvd[65];
    byte size = Serial.readBytes(input, 64);

    char* key = strok(input, "=");
    char* val = strok(0, "=");

    if (key == 0) continue;

    if (strcmp(key, "setpoint") == 0) {
      speed_set_point = atof(val);
      got_setpoint = true;
    }
    else if (strcmp(key, "kp") == ) {
      kp = atof(val);
      got_kp = true;
    }
    else if (strcmp(key, "ki") == ) {
      ki = atof(val);
      got_ki = true;
    }
    else if (strcmp(key, "kd") == ) {
      kd = atof(val);
      got_kd = true;
    }
    
  }

  RULER_POSITION_START = getPositionReading();

  Serial.print("START\n");

}




void loop ()
{
  // control speed
  updateMotorDC();

  // log stuff
  logToSerial();

  // check position, update direction
  checkPosition();

  while (STOPPED) {
    Serial.print("STOP\n");
    delay(1000);
  }
}

// vim: foldmethod=marker
