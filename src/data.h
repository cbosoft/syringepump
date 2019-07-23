#pragma once
#include <gtk/gtk.h>

struct Data {
  int serial_fd;
  char *serial_path;
  int res;

  GObject *main_win;
  GObject *conn_btn;
  GObject *disconn_btn;
  GObject *refresh_btn;
  GObject *setpoint_inp;
  GObject *kp_inp;
  GObject *ki_inp;
  GObject *kd_inp;
  GObject *log_lbl;
  GObject *scroll;
  GObject *serial_cmb;
};

struct Data *new_data(
    int serial_fd,
    char *serial_path,
    int res,
    GObject *main_win, 
    GObject *conn_btn,
    GObject *disconn_btn, 
    GObject *refresh_btn, 
    GObject *setpoint_inp,
    GObject *kp_inp,
    GObject *ki_inp,
    GObject *kd_inp,
    GObject *log_lbl,
    GObject *scroll,
    GObject *serial_cmb);

// vim: ft=c
