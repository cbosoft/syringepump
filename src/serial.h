#pragma once

#include <gtk/gtk.h>

#include "data.h"

int wait_for(struct Data *data, const char *trigger, int number, int *flagaddr, int stopval);

int open_serial(const char *serial_path, struct Data *data);
int close_serial(struct Data *data);

int read_serial_until(struct Data *data, char* buf, int buf_max, char until, int timeout);
int read_serial_line(struct Data *data, char* buf, int buf_max, int timeout);
int write_serial(struct Data *data, char* buf, int buf_max);
void send_data_packet(struct Data *data, const char *key, const char *value);

// vim: ft=c
