#pragma once
#include <gtk/gtk.h>

struct connect_data {
  int serial_fd;
  char *serial_path;
  int res;

  GObject *main_win;
  GObject *conn_btn;
  GObject *disconn_btn;
  GObject *log_lbl;
  GObject *scroll;
};

struct connect_data *create_cd(
    int serial_fd, 
    char *serial_path, 
    int res, 
    GObject *main_win, 
    GObject *conn_btn,
    GObject *disconn_btn, 
    GObject *log_lbl,
    GObject *scroll);

void cb_connect(GObject *obj, struct connect_data *cd);
void cb_disconnect(GObject *obj, struct connect_data *cd);
void cb_quit(GObject *obj, struct connect_data *cd);

// vim: ft=c
