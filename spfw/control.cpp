#include <Arduino.h>
#include "control.h"

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
int dc = 0.0;
enum controllers {
  CONTROL_UNSET,
  CONTROL_PID,
  CONTROL_NONE
};
int control_type = CONTROL_UNSET;

int getControlAction(double speed)
{
  // TODO

  if (control_type == CONTROL_NONE) {
    return dc;
  }
  
  return 120;
}


void controlInit(){

  int params_got = 0;
  int params_needed = 1;
  Serial.print("WAIT\n");

  while (params_got < params_needed) {

    while (!Serial.available()) delay(100);

    char recvd[65];
    byte size = Serial.readBytes(recvd, 64);

    char* key = strtok(recvd, "=");
    char* val = strtok(0, "=");

    if (key == 0) continue;

    if (strcmp(key, "setpoint") == 0) {
      control_type = CONTROL_PID;
      speed_set_point = atof(val);
    }
    else if (strcmp(key, "kp") == 0) {
      control_type = CONTROL_PID;
      kp = atof(val);
    }
    else if (strcmp(key, "ki") == 0) {
      control_type = CONTROL_PID;
      ki = atof(val);
    }
    else if (strcmp(key, "kd") == 0) {
      control_type = CONTROL_PID;
      kd = atof(val);
    }
    else if (strcmp(key, "dc") == 0) {
      control_type = CONTROL_NONE;
      dc = atoi(val);
    }

    params_got ++;

    switch (control_type) {
      case CONTROL_PID:
        params_needed = 4;
        break;
      case CONTROL_UNSET:
      case CONTROL_NONE:
        params_needed = 1;
        break;
    }
    Serial.print("OK\n");

  } 
}

// vim: ft=arduino
