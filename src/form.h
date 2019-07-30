#pragma once
#include "data.h"

enum FORM_SENSITIVITIES {
  FORM_BUSY,
  FORM_CONNECTED,
  FORM_DISCONNECTED,
  FORM_ALL
};

int check_form(struct Data *data);
void form_set_sensitive(int sensitivity_flag);

// vim: ft=c
