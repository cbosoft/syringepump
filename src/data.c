#include <gtk/gtk.h>
#include "data.h"

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
    GObject *serial_cmb)
{
  struct Data * data = malloc(sizeof(struct Data));
  
  data->serial_fd = -1;
  data->serial_path = "/dev/ttyACM0";
  data->res = 0;
  data->main_win = main_win;
  
  // buttons
  data->conn_btn = conn_btn;
  data->disconn_btn = disconn_btn;
  data->refresh_btn = refresh_btn;

  // inputs
  data->setpoint_inp = setpoint_inp;
  data->kp_inp = kp_inp;
  data->ki_inp = ki_inp;
  data->kd_inp = kd_inp;

  data->log_lbl = log_lbl;
  data->scroll = scroll;
  data->serial_cmb = serial_cmb;

  return data;
}
