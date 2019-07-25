#ifndef __MOTOR__
#define __MOTOR__

// define pins used
#define MOTOR_DIR_PIN_1 4
#define MOTOR_PWM_PIN 5
#define MOTOR_DIR_PIN_2 6

// define directions
#define MOTOR_REVERSE 0
#define MOTOR_FORWARD 1

void motorSetDirection(int dir);
void motorSetDC(unsigned int dc);
void motorInit();

#endif
