#pragma once
#include <gtk/gtk.h>

struct Data {

  // Serial
  int serial_fd;
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
  GObject *tag_inp;
  GObject *log_lbl;
  GObject *scroll;
  GObject *serial_cmb;
};

// vim: ft=c
