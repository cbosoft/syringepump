#pragma once
#include "data.h"

enum FORM_SENSITIVITIES
  {
   FORM_BUSY,
   FORM_CONNECTED,
   FORM_DISCONNECTED,
   FORM_REFRESHING,
   FORM_NOSERIAL,
   FORM_ALL
  };

enum CONTROL_PAGES
  {
   PAGE_PID_FLOW,
   PAGE_PID_FORCE,
   PAGE_NO_CONTROL
  };

int check_form(struct Data *data);
void form_set_sensitive(struct Data *data, int sensitivity_flag);
void form_set_progress(struct Data *data, double fraction);
void form_pulse_progress(struct Data *data);

// vim: ft=c
