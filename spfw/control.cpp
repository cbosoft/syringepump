#include "Arduino.h"
#include "control.h"
#include "setter.h"

unsigned long ptime = 0;
double dc = 0.0;
double buflen = 0.0;
double inner_diameter = 0.0;
int log_options = 56; // 111000


extern unsigned long time;

extern setter_func setter;

extern double sine_frequency;
extern double sine_mean;
extern double sine_magnitude;

extern double ramp_gradient;
extern double ramp_intercept;

extern double constant_val;

extern double step_initial;
extern double step_time;
extern double step_final;






double PIDController::get_action(double setpoint, double flowrate, double force)
{
  double delta_t = (double)(time - ptime)*0.001; // in s
  ptime = time;

  double dca = 0.0;

  // Calculate error
  double input = 0.0;
  switch (this->controlled_variable) {
    
  case CONTROLLED_FLOWRATE:
    input = flowrate;
    break;
    
  case CONTROLLED_FORCE:
    input = force;
    break;
    
  }
  double err = setpoint - input;

  // Proportional control
  if (this->pid_kp)
    dca += this->pid_kp * (err - this->err1);

  // Integral control
  if (this->pid_ki)
    dca += this->pid_ki * err * delta_t;

  // Derivative control
  if ((this->pid_kd) && (delta_t != 0.0))
    dca += this->pid_kd * (err - (2*this->err1) + this->err2) / delta_t;

  // Update Error history
  this->err2 = err1;
  this->err1 = err;

  this->previous_ca = this->ca;
  this->ca = this->previous_ca + dca;
  if (this->ca > MAX_DC)
    this->ca = MAX_DC;
  else if (this->ca < MIN_DC)
    this->ca = MIN_DC;

  return this->ca;
}

double MeasureController::get_action(double setpoint, double flowrate, double force)
{
  if ((time*0.001) > this->measure_time)
    return this->ca;

  double delta_t = (double)(time - ptime)*0.001; // in s
  ptime = time;


  // Calculate error
  double input = 0.0;
  switch (this->controlled_variable) {
    
  case CONTROLLED_FLOWRATE:
    input = flowrate;
    break;
    
  case CONTROLLED_FORCE:
    input = force;
    break;
    
  }
  double err = setpoint - input;

  double dca = 0.0;
  // Proportional control
  if (this->pid_kp)
    dca += this->pid_kp * (err - this->err1);

  // Integral control
  if (this->pid_ki)
    dca += this->pid_ki * err * delta_t;

  // Derivative control
  if ((this->pid_kd) && (delta_t != 0.0))
    dca += this->pid_kd * (err - (2*this->err1) + this->err2) / delta_t;

  // Update Error history
  this->err2 = err1;
  this->err1 = err;

  this->previous_ca = this->ca;
  this->ca = this->previous_ca + dca;
  if (this->ca > MAX_DC)
    this->ca = MAX_DC;
  else if (this->ca < MIN_DC)
    this->ca = MIN_DC;

  return this->ca;
}




// double MeasureController::get_action(double setpoint, double flowrate, double force)
// {
//   // if measure period is over or only been a few loops, just return a constant.
//   if (((time*0.001) > this->measure_time) || (this->passive--))
//     return this->ca;
// 
//   this->passive = 5;
// 
//   // otherwise, measure how control affects the controlled variable.
//   double input = 0.0;
//   switch (this->controlled_variable) {
//     
//   case CONTROLLED_FLOWRATE:
//     input = flowrate;
//     break;
//     
//   case CONTROLLED_FORCE:
//     input = force;
//     break;
//     
//   }
// 
//   // // moving average of window length n
//   // if (!this->filled) {
//   //   this->inputs = realloc(this->inputs, (++this->n)*sizeof(char));
//   //   this->inputs[this->n] = input;
//   //   if (this->n == this->max_n-1) {
//   //     this->filled = 1;
//   //   }
//   // }
//   // else {
//   //   this->n = (++this->n) % this->max_n;
//   //   this->inputs[this->n] = input;
//   // }
//   // double total = 0.0;
//   // Serial.print("   INPUT: ");
//   // Serial.print(input);
// 
//   // if (!this->filled) {
//   //   for (int i = 0; i < this->n; i++) total += this->inputs[i];
//   //   input = total / ((double)this->n);
//   // }
//   // else {
//   //   for (int i = 0; i < this->max_n; i++) total += this->inputs[i];
//   //   input = total / ((double)this->max_n);
//   // }
//   // Serial.print("   AVINPUT: ");
//   // Serial.print(input);
//   // Serial.print("\n");
// 
// 
// 
//   if (input < setpoint) {
//     this->ca += 1;
//   }
//   else if (input > setpoint) {
//     this->ca -= 1;
//   }
// 
//   delay(100);
// 
//   return this->ca;
// }




Controller *controlInit(){

  int done = 0,
    sp_recvd = 0,
    tp_recvd = 0,
    bd_recvd = 0,
    lo_recvd = 0,
    controlled_var = 0,
    control_type = CONTROL_NONE;

  Serial.print("WAIT\n");
  Controller *rv = NULL;

  for (int i = 0; i < 10 && !done; i++) {

    while (!Serial.available());

    char recvd[65];
    byte size = Serial.readBytes(recvd, 64);

    char* key = strtok(recvd, "=");
    char* val = strtok(0, "=");

    if (key == 0) continue;

    if (strcmp(key, "SP") == 0) {

      if ((val[0] == 'F') || (val[0] == 'Q')) {
        controlled_var = val[0] == 'Q' ? CONTROLLED_FLOWRATE : CONTROLLED_FORCE;
      }
      val++;

      char setter_ch = val[0];
      char *param;
      val++;
      
      switch (setter_ch) {
      case 'S':
        setter = &sine;

        param = strtok(val, ",");
        sine_frequency = atof(param);

        param = strtok(0, ",");
        sine_magnitude = atof(param);

        param = strtok(0, ",");
        sine_mean = atof(param);
        break;

      case 'R':
        setter = &ramp;

        param = strtok(val, ",");
        ramp_gradient = atof(param);

        param = strtok(0, ",");
        ramp_intercept = atof(param);
        break;

      case 'T':
        setter = &step;

        param = strtok(val, ",");
        step_initial = atof(param);

        param = strtok(0, ",");
        step_time = atof(param);

        param = strtok(0, ",");
        step_final = atof(param);
        break;

      default:
      case 'C':
        setter = &constant;
        constant_val = atof(val);
        break;
      }

      sp_recvd = 1;
    }
    else if (strcmp(key, "TP") == 0) {

      char ct = val[0], *param;
      double kp, ki, kd;
      unsigned long meas_time;
      switch (ct) {

      case 'P':
	      control_type = CONTROL_PID;

	      param = strtok(++val, ",");
	      kp = atof(param);
	      param = strtok(0, ",");
	      ki = atof(param);
	      param = strtok(0, ",");
	      kd = atof(param);
	      rv = new PIDController(kp, ki, kd);
	      break;

      case 'M':
	      control_type = CONTROL_MEAS;
	      param = strtok(++val, ",");
	      kp = atof(param);
	      param = strtok(0, ",");
	      ki = atof(param);
	      param = strtok(0, ",");
	      kd = atof(param);
	      rv = new MeasureController(kp, ki, kd);
	      break;
      }

      tp_recvd = 1;
    }
    else if (strcmp(key, "BD") == 0) {
      char *data = strtok(val, ",");
      buflen = atof(data);
      data = strtok(0, ",");
      inner_diameter = atof(data);

      bd_recvd = 1;
    }
    else if (strcmp(key, "LO") == 0) {
      log_options = atoi(val);

      lo_recvd = 1;
    }

    switch (control_type) {

    case CONTROL_PID:
    case CONTROL_MEAS:
      if (sp_recvd && tp_recvd && lo_recvd && bd_recvd)
        done = 1;
      break;

    case CONTROL_NONE:
      if (sp_recvd && lo_recvd && bd_recvd)
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

  if (rv == NULL)
    rv = new NoController();

  rv->set_controlled_variable(controlled_var);

  return rv;
}

// vim: ft=arduino
