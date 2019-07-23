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
    GObject *log_lbl,
    GObject *scroll,
    GObject *serial_cmb);

// vim: ft=c
