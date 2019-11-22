#include <Arduino.h>
#include "control.h"

double setpoint = 0;
double kp = 0.2;
double ki = 0.0;
double kd = 0.0;
unsigned long ptime = 0;
double dc = 0.0;
double buflen = 0.0;
double inner_diameter = 0.0;
enum controllers {
  CONTROL_UNSET,
  CONTROL_PID,
  CONTROL_NONE
};
int control_type = CONTROL_UNSET;
int controlled_var = 0;
int raw_lc = 0;
double err1 = 0.0;
double err2 = 0.0;

const double max_ca = 255.0;
const double min_ca = 0.0;









int ccntr = 0;
double getControlAction(double pca, double speed, double force)
{

  if (control_type == CONTROL_NONE) {
    return dc;
  }

  unsigned long timenow = millis();
  double delta_t = (double)(timenow - ptime);

  double dca = 0.0;

  // Calculate error
  double input = controlled_var ? speed : force;
  double err = setpoint - input;

  // Proportional control
  dca += kp * (err - err1);

  // Integral control
  dca += ki * err * delta_t;

  // Derivative control
  dca += kd * (err - (2*err1) + err2) / delta_t;

  // Update Error history
  err2 = err1;
  err1 = err;

  double ca = pca + dca;
  if (ca > max_ca)
    ca = max_ca;
  else if (ca < min_ca)
    ca = min_ca;

  return ca;
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
      setpoint = atof(val);
    }
    else if (strcmp(key, "var") == 0) {
      control_type = CONTROL_PID;
      controlled_var = strcmp(val, "force") != 0;
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
      dc = atof(val);
    }
    else if (strcmp(key, "bl") == 0) {
      buflen = atof(val);
    }
    else if (strcmp(key, "di") == 0) {
      inner_diameter = atof(val);
    }
    else if (strcmp(key, "raw") == 0) {
      raw_lc = atoi(val);
    }

    params_got ++;

    switch (control_type) {
      case CONTROL_PID:
        params_needed = 8;
        break;
      case CONTROL_UNSET:
      case CONTROL_NONE:
        params_needed = 4;
        break;
    }
    Serial.print("OK\n");

  } 
}

// vim: ft=arduino
