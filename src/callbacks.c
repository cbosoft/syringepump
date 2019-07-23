#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "callbacks.h"
#include "ardiop.h"
#include "error.h"
#include "log.h"
#include "serial.h"

extern int LOG_STOPPED;
extern pthread_t log_thread;



void cb_connect(GObject *obj, struct Data *data)
{
  data->serial_path = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(data->serial_cmb));

  timestamp(data, "connecting to \"%s\"", data->serial_path);
  data->serial_fd = ard_openserial(data->serial_path);

  if (data->serial_fd < 0) {
    timestamp(data, "connecting to \"%s\"", data->serial_path);

    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(data->main_win), 
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, 
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_CLOSE, 
        "Could not connect on \"%s\"\n(%d) %s", 
        data->serial_path, 
        errno, 
        strerror(errno));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  pthread_create(&log_thread, NULL, log_update, &data);

  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);

}




void cb_disconnect(GObject *obj, struct Data *data)
{

  LOG_STOPPED = 1;
  pthread_join(log_thread, NULL);

  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 1);

  close(data->serial_fd);

  timestamp(data, "disconnected");

}



void cb_quit(GObject *obj, struct Data *data)
{
  
  timestamp(data, "closing...");

  if (!LOG_STOPPED)
    cb_disconnect(NULL, data);

  gtk_main_quit();

}



void cb_refresh_serial(GObject *obj, struct Data *data )
{
  refresh_serial_list(data);
}




