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
    GObject *log_lbl,
    GObject *scroll,
    GObject *serial_cmb)
{
  struct Data * data = malloc(sizeof(struct Data));
  
  data->serial_fd = -1;
  data->serial_path = "/dev/ttyACM0";
  data->res = 0;
  data->main_win = main_win;
  data->conn_btn = conn_btn;
  data->disconn_btn = disconn_btn;
  data->refresh_btn = refresh_btn;
  data->log_lbl = log_lbl;
  data->scroll = scroll;
  data->serial_cmb = serial_cmb;

  return data;
}
