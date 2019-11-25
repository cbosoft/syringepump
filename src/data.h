#pragma once
#include <gtk/gtk.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

struct Data {

#ifdef WINDOWS
  HANDLE serial_handle;
#else
  // Serial
  int serial_fd;
#endif
  char *serial_path;

  // ??
  int res;

  // logging
  char *tag;
  char *logpath;

  GtkBuilder *builder;

  // worker status
  int refresh_worker_status;
  int log_worker_status;
  int connect_worker_status;
};

// vim: ft=c
