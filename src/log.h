#pragma once
#include <gtk/gtk.h>

struct thread_data {
  int serial_fd;

  GObject *log_lbl;
  GObject *scroll;
};

void *log_update(void *vptr);

// vim: ft=c