#pragma once
#include "data.h"

enum FORM_SENSITIVITIES {
  FORM_BUSY,
  FORM_CONNECTED,
  FORM_DISCONNECTED,
  FORM_REFRESHING,
  FORM_NOSERIAL,
  FORM_ALL
};

int check_form(struct Data *data);
void form_set_sensitive(struct Data *data, int sensitivity_flag);
void form_set_progress(struct Data *data, double fraction);
void form_pulse_progress(struct Data *data);

// vim: ft=c
