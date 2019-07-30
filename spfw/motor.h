#ifndef __MOTOR__
#define __MOTOR__

// define pins used
#define MOTOR_PWM_PIN 3
#define MOTOR_DIR_PIN_1 4
#define MOTOR_DIR_PIN_2 5

// define directions
#define MOTOR_REVERSE 0
#define MOTOR_FORWARD 1

void motorSetDirection(int dir);
void motorSetDC(int dc);
void motorInit();

#endif
