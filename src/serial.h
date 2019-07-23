#pragma once
#include <gtk/gtk.h>
#include "data.h"

#define CAST(VPTR, TYPE) (*(TYPE *)VPTR)
#define T_STR 0
#define T_FLOAT 1
#define T_INT 2
#define ARDUINO_MESG_LEN 64

void refresh_serial_list(struct Data *data);
void send_key_value_to_arduino(struct Data *data, const char *key, void *val_vptr, int type);

// vim: ft=c
