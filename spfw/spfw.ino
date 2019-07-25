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

#define SPEED_HIST_LEN 32
#define ERR_HIST_LEN 1000
double speed_set_point = 0;
double speed_current = 0;
double speed_average = 0;
double speed_hist[SPEED_HIST_LEN] = {0};
int speed_hist_count = 0;
double err_hist[ERR_HIST_LEN] = {0};
unsigned int err_count = 0;
double kp = 0.2;
double ki = 0.0;
double kd = 0.0;

// }}}
// LOAD CELL {{{

HX711 loadcell;
const int LC_DOUT = 7;
const int LC_CLK = 8;
const long LC_OFFSET = 0;
const long LC_DIVIDER = 1;

// }}}
// RULERS {{{
const double RULER_POSITION_CAL_M = -0.12646004;
const double RULER_POSITION_CAL_C = 124.12431658;
const int RULER_POSITION_PIN = A0;
const int RULER_POSITION_END = 960;
int RULER_POSITION_START = 0;

long position_byte = 0;
long time_position_read = 0;

const int RULER_DIAMETER_PIN = A1;
// }}}
// OPTICAL ENCODER {{{

// Arduino UNO has two hardware interrupt pins: 2, 3;
#define OPT_PIN 2
#define TIME_LEN 10
static unsigned long time_hist[TIME_LEN] = {0};
static unsigned int time_count = 0;
const double OPT_MASK_ARC_LEN = PI*2.0*0.25; // quarter turn
// }}}

extern volatile unsigned long timer0_millis;
bool STOPPED = false;
bool REVERSING = false;




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

double byte_pos_to_mm(long byte_pos)
{
  double byte_pos_f = double(byte_pos);

  return (byte_pos_f * RULER_POSITION_CAL_M) + RULER_POSITION_CAL_C;
}


double getSpeedReading()
{
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




void logToSerial() {
  // time since start
  Serial.print(millis());
  Serial.print(",");
  
  // load cell
  Serial.print(getLoadCellReading());
  Serial.print(",");
  
  // position (ruler)
  Serial.print(position_byte);
  Serial.print(",");
  
  // diameter (ruler)
  Serial.print(getDiameterReading());
  Serial.print(",");

  // speed
  Serial.print(speed_current);
  Serial.print(",");
  Serial.print(speed_average);
  Serial.print("\n");
}



void checkPosition()
{

  if (position_byte > RULER_POSITION_END) {
    motorSetDC(0);
    STOPPED = true;
  }

}

int ledstatus = 0;
void optMark(void)
{
  ledstatus = !ledstatus;
  digitalWrite(13, ledstatus);
}


void setup () 
{
  Serial.begin(9600);
  Serial.print("ON\n");

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

  // Optical encoder
  pinMode(OPT_PIN, INPUT);
  digitalWrite(OPT_PIN, HIGH);
  pinMode(13, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);

  bool got_setpoint = false;
  bool got_kp = false;
  bool got_ki = false;
  bool got_kd = false;

  Serial.print("WAIT\n");

  while (!got_setpoint || !got_kp || !got_ki || !got_kd) {

    while (!Serial.available()) delay(100);

    char recvd[65];
    byte size = Serial.readBytes(recvd, 64);

    char* key = strtok(recvd, "=");
    char* val = strtok(0, "=");

    if (key == 0) continue;

    if (strcmp(key, "setpoint") == 0) {
      speed_set_point = atof(val);
      got_setpoint = true;
    }
    else if (strcmp(key, "kp") == 0) {
      kp = atof(val);
      got_kp = true;
    }
    else if (strcmp(key, "ki") == 0) {
      ki = atof(val);
      got_ki = true;
    }
    else if (strcmp(key, "kd") == 0) {
      kd = atof(val);
      got_kd = true;
    }

    Serial.print("OK\n");

  } 

  // reset millis counter to zero
  noInterrupts();
  timer0_millis = 0;
  interrupts();

  Serial.print("START\n");

  RULER_POSITION_START = getPositionReading();
  analogWrite(MOTOR_PWM_PIN, 120);

}


void loop ()
{
  delay(1000);
  long new_position_byte = getPositionReading();
  long new_time_position_read = millis();

  long dr = new_position_byte - position_byte;
  long dt = new_time_position_read - time_position_read;

  if (dt == 0)
    Serial.print("DT IS ZERO");

  position_byte = new_position_byte;
  time_position_read = new_time_position_read;

  speed_current = double(1000*dr)/double(dt);

  if (speed_hist_count < SPEED_HIST_LEN){
    speed_hist[speed_hist_count] = speed_current;
    speed_hist_count ++;
  }
  else {
    for (int i = 0; i < SPEED_HIST_LEN-1; i++)
      speed_hist[i] = speed_hist[i+i];
    speed_hist[SPEED_HIST_LEN-1] = speed_current;
  }
  double total = 0.0;
  for (int i = 0; i < speed_hist_count; i++) {
    total += speed_hist[i];
  }
  speed_average = total / double(speed_hist_count);
  Serial.print(speed_current);
  Serial.print(", ");
  Serial.print(total);
  Serial.print(", ");
  Serial.print(speed_average);
  Serial.print("\n");

  // control speed
  updateMotorDC();

  // log stuff
  //logToSerial();

  // check position, update direction
  checkPosition();

  while (STOPPED) {
    Serial.print("STOP\n");
    delay(1000);
  }
}

// vim: foldmethod=marker
