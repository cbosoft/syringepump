#pragma once
#include <gtk/gtk.h>

#include "callbacks.h"

struct thread_data {
  int serial_fd;

  GObject *log_lbl;
  GObject *scroll;
};

void *log_update(void *vptr);
void append_text_to_log(struct connect_data *cd, const char *added_text);

// vim: ft=c
