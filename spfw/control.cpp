#include <Arduino.h>
#include "control.h"

double speed_set_point = 0;
double kp = 0.2;
double ki = 0.0;
double kd = 0.0;
double dc = 0.0;
double buflen = 0.0;
enum controllers {
  CONTROL_UNSET,
  CONTROL_PID,
  CONTROL_NONE
};
int control_type = CONTROL_UNSET;




typedef struct ll_control_hist {
  struct ll_control_hist *next;
  double error;
  double time;
} ll_control_hist;
ll_control_hist *control_hist = NULL;




ll_control_hist *remove_lru(ll_control_hist *start)
{
  ll_control_hist *rv = start->next;
  free(start);
  return rv;
}




ll_control_hist *append_to_max(ll_control_hist *start, double error, double time)
{
  ll_control_hist *new_item = calloc(1, sizeof(ll_control_hist));
  new_item->error = error;
  new_item->time = time;

  if (start == NULL)
    return new_item;

  int i = 0;
  ll_control_hist *last;
  for (last = start; last->next != NULL; last = last->next, i++);
  last->next = new_item;

  if (i >= ERR_HIST_LEN) {
    return remove_lru(start);
  }
  
  return start;
}

double integrate_hist(ll_control_hist *start)
{
  if (start->next == NULL) {
    return 0.0;
  }

  double total = 0.0;
  ll_control_hist *item = start, *next = start->next;
  while (next->next != NULL) {
    total += item->error * (next->time - item->time);
    item = item->next;
    next = next->next;
  }
  return total;
}




int ccntr = 0;
double getControlAction(double pca, double speed)
{

  if (control_type == CONTROL_NONE) {
    return dc;
  }

  double dca = 0.0;

  // Calculate error
  double error = speed_set_point - speed;
  control_hist = append_to_max(control_hist, error, double(millis())*0.001 );

  // proportional
  dca += kp * error;

  // integral
  dca += ki * integrate_hist(control_hist);

  // TODO derivative control

  return pca + dca;
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
      dc = atof(val);
    }
    else if (strcmp(key, "bl") == 0) {
      buflen = atof(val);
    }

    params_got ++;

    switch (control_type) {
      case CONTROL_PID:
        params_needed = 5;
        break;
      case CONTROL_UNSET:
      case CONTROL_NONE:
        params_needed = 2;
        break;
    }
    Serial.print("OK\n");

  } 
}

// vim: ft=arduino
