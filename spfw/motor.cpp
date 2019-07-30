#include <Arduino.h>
#include "motor.h"

void motorSetDirection(int dir)
{
  digitalWrite(MOTOR_DIR_PIN_1, dir);
  digitalWrite(MOTOR_DIR_PIN_2, !dir);
}

void motorSetDC(int dc)
{
  if (dc > 255) {
    dc = 255;
  }

  if (dc < 0) {
    dc = 0;
  }

  analogWrite(MOTOR_PWM_PIN, dc);
}

void motorInit()
{
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN_1, OUTPUT);
  pinMode(MOTOR_DIR_PIN_2, OUTPUT);
  motorSetDirection(1);
  motorSetDC(0);
}

// vim: ft=arduino
