#include <errno.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "error.h"
#include "log.h"
#include "serial.h"
#include "form.h"
#include "connect.h"
#include "disconnect.h"
#include "refresh.h"




void cb_lbl_size_changed(GObject *obj, GdkRectangle *allocation, struct Data *data)
{
  // log_lbl size is changed
  GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(data->scroll));

  gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}




void cb_begin_clicked(GObject *obj, struct Data *data)
{
  // button "Begin!" clicked
  connect_to(data);
}




void cb_stop_clicked(GObject *obj, struct Data *data)
{
  // button "Stop" clicked
  disconnect(data, 1);
}




void cb_quit_clicked(GObject *obj, struct Data *data)
{

  disconnect(data, 1);
  timestamp(data, 1, "Closing...");
  gtk_main_quit();

}



void cb_refresh_clicked(GObject *obj, struct Data *data )
{
  refresh(data);
}




void cb_tag_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}




void cb_dc_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}



void cb_kp_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}




void cb_ki_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}




void cb_kd_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}




void cb_setpoint_text_changed(GObject *obj, struct Data *data)
{
  get_new_log_name(data, NULL);
}




void cb_tab_page_changed(GObject *obj, GtkWidget *page, guint page_num, struct Data *data)
{
  int n = (int)page_num;
  get_new_log_name(data, &n);
}
