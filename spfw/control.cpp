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

int getControlAction(double speed)
{
  // TODO
  return 120;
}


void controlInit(){

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
}

// vim: ft=arduino
