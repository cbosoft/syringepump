#include <SPI.h>
#include "hx711.h"

// MOTOR

// There are 6 pwm pins on the Arduino UNO: pins 3, 5, 6, 9, 10, and 11.
const int MOTOR_PWM_PIN = 4;

// Motor direction is controlled by H-bridge: two transistors control the
// direction. If PIN_1 is high and PIN_2 is low... TODO
const int MOTOR_DIR_PIN_1 = 5;
const int MOTOR_DIR_PIN_2 = 6;

static long speed_set_point = 0;
static long speed_current = 0;


// LOAD CELL
HX711 loadcell;
const int LC_DOUT = 7;
const int LC_CLK = 8;
const long LC_OFFSET = 0;
const long LC_DIVIDER = 1;

// RULERS
const int RULER_LENGTH_PIN = A0;
const int RULER_DIAMETER_PIN = A1;


// OPTICAL ENCODER
// Arduino UNO has two hardware interrupt pins: 2, 3;
#define OPT_PIN 2
#define TIME_LEN 10
static unsigned long times[TIME_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned int time_counts = 0;
const float OPT_MASK_ARC_LEN = PI*2.0*0.25; // quarter turn

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
  return analogRead(RULER_LENGTH_PIN);
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

  // Optical encoder setup
  pinMode(OPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(OPT_PIN), optMark, CHANGE);

  Serial.print("START\n");

}

void loop ()
{
  // control speed
  updateMotorDC();

  // log stuff
  logToSerial();
}
