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
int log_options = 56; // 111000
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

  int done = 0,
    sp_recvd = 0,
    tp_recvd = 0,
    dc_recvd = 0,
    bf_recvd = 0,
    di_recvd = 0,
    lo_recvd = 0;

  Serial.print("WAIT\n");

  for (int i = 0; i < 100 && !done; i++) {

    while (!Serial.available()) delay(100);

    char recvd[65];
    byte size = Serial.readBytes(recvd, 64);

    char* key = strtok(recvd, "=");
    char* val = strtok(0, "=");

    if (key == 0) continue;

    if (strcmp(key, "SP") == 0) {
      control_type = CONTROL_PID;

      controlled_var = val[0] != 'F';
      val++;

      setpoint = atof(val);

      sp_recvd = 1;
    }
    else if (strcmp(key, "TP") == 0) {
      control_type = CONTROL_PID;

      char *param = strtok(val, ",");
      kp = atof(param);
      param = strtok(0, ",");
      ki = atof(param);
      param = strtok(0, ",");
      kd = atof(param);

      tp_recvd = 1;
    }
    else if (strcmp(key, "DC") == 0) {
      control_type = CONTROL_NONE;
      dc = atof(val);

      dc_recvd = 1;
    }
    else if (strcmp(key, "BF") == 0) {
      buflen = atof(val);

      bf_recvd = 1;
    }
    else if (strcmp(key, "DI") == 0) {
      inner_diameter = atof(val);

      di_recvd = 1;
    }
    else if (strcmp(key, "LO") == 0) {
      log_options = atoi(val);

      lo_recvd = 1;
    }

    switch (control_type) {

    case CONTROL_PID:
      if (sp_recvd && tp_recvd && lo_recvd && di_recvd && bf_recvd)
        done = 1;
      break;

    case CONTROL_NONE:
      if (dc_recvd && lo_recvd && di_recvd && bf_recvd)
        done = 1;
      break;

    }

    Serial.print("OK\n");

  } 

  if (!done) {
    while (1) {
      Serial.print("STOP\n");
      Serial.print("Incorrect parameter set given.\n");
    }
  }
}

// vim: ft=arduino
