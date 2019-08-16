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

  // GUI
  GObject *main_win;
  GObject *conn_btn;
  GObject *disconn_btn;
  GObject *refresh_btn;
  GObject *setpoint_inp;
  GObject *kp_inp;
  GObject *ki_inp;
  GObject *kd_inp;
  GObject *buflen_inp;
  GObject *tag_inp;
  GObject *log_folder_fch;
  GObject *log_lbl;
  GObject *logname_lbl;
  GObject *progress;
  GObject *scroll;
  GObject *serial_cmb;
  GObject *control_tab;
  GObject *dc_inp;

  // worker status
  int refresh_worker_status;
  int log_worker_status;
  int connect_worker_status;
};

// vim: ft=c
