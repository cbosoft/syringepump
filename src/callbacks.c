#include <errno.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "ardiop.h"
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
  connect(data);
}




void cb_disconnect(GObject *obj, struct Data *data)
{

  LOG_STOPPED = 1;
  g_thread_join(log_thread);

#ifndef WINDOWS
  ard_writeserial(data->serial_fd, "QUIT", 4);
  if (data->serial_fd > 0)
    close(data->serial_fd);
#else
  //TODO
  // tell arduino to reset, destroy HANDLE appropriately
#endif

  timestamp(data, "disconnected");

}



void cb_quit(GObject *obj, struct Data *data)
{
  
  timestamp(data, "Closing...");

  if (!LOG_STOPPED)
    cb_disconnect(NULL, data);

  gtk_main_quit();

}



void cb_refresh_serial(GObject *obj, struct Data *data )
{
  refresh(data);
}




