#pragma once
#include "data.h"

typedef enum FORM_SENSITIVITIES {
 FORM_BUSY,
 FORM_CONNECTED,
 FORM_DISCONNECTED,
 FORM_REFRESHING,
 FORM_NOSERIAL,
 FORM_ALL
} FORM_SENSITIVITIES;

typedef enum FORM_CONTROL_SELECTION {
  FORM_CONTROL_NONE,
  FORM_CONTROL_MEAS,
  FORM_CONTROL_PID
} FORM_CONTROL_SELECTION;

typedef enum FORM_SETTER_SELECTION {
  FORM_SETTER_CONSTANT,
  FORM_SETTER_RAMP,
  FORM_SETTER_STEP,
  FORM_SETTER_SINE
} FORM_SETTER_SELECTION;

typedef enum FORM_CONTROLLED_VAR {
  FORM_VAR_FLOW,
  FORM_VAR_FORCE
} FORM_CONTROLLED_VAR;

int check_form(struct Data *data);
void form_set_sensitive(struct Data *data, int sensitivity_flag);
void form_set_progress(struct Data *data, double fraction);
void form_pulse_progress(struct Data *data);
FORM_CONTROL_SELECTION form_get_control_type(struct Data *data);
FORM_SETTER_SELECTION form_get_setter_type(struct Data *data);
FORM_CONTROLLED_VAR form_get_controlled_var(struct Data *data);
void form_setter_update(struct Data *data);
int form_get_log_options(struct Data *data);
char *form_get_const_setter_params(struct Data *data);
char *form_get_ramp_setter_params(struct Data *data);
char *form_get_step_setter_params(struct Data *data);
char *form_get_sine_setter_params(struct Data *data);
char *form_get_pid_params(struct Data *data);
char *form_get_meas_params(struct Data *data);
char *form_get_bldi_data(struct Data *data);
GObject * get_object_safe(struct Data *data, const char *name);

// vim: ft=c
