#include <errno.h>
#include <string.h>

#include "callbacks.h"
#include "ardiop.h"
#include "error.h"
#include "log.h"
#include "serial.h"

extern int LOG_STOPPED;

static GThread *log_thread;
static GThread *connect_thread;




static void *log_update_thread(void *void_data)
{ 
  struct Data *data = (struct Data *)void_data;
  
  timestamp(data, "Waiting for Arduino...");
  wait_for(data, "START", 100);
  timestamp(data, "Arduino ready, starting!");

  // TODO open log file

  int i = 0, timeout = 1000;
  LOG_STOPPED = 0;
  
  struct timespec ms_span;
  ms_span.tv_sec = 0;
  ms_span.tv_nsec = 1000*1000;

  while (!LOG_STOPPED) {
    char received_text[512] = {0};
    
    char b[1];
    do {
      int n = read(data->serial_fd, b, 1);

      if (LOG_STOPPED)
        break;

      if(n == -1) {
        timestamp(NULL, "something went wrong reading a byte (read failed) (%d) %s", errno, strerror(errno));
        exit(0); // TODO handle properly
      }

      if(n == 0) {
        nanosleep(&ms_span, NULL);  // wait 1 msec try again
        timeout--;
        if(timeout == 0){
          timestamp(NULL, "something went wrong reading a byte (timeout exceeded) (%d), %s", errno, strerror(errno));
          exit(0); // TODO handle properly
        }

        continue;
      }

      received_text[i] = b[0];

      i++;

    } while( b[0] != '\n' && i < 512);

    timestamp( ( (i % 10) == 0) ? data : NULL, "R: %s", received_text);

    i = (i % 10 == 0) ? (1) : (i + 1);

    // write to log file
    // TODO
  }

  // post log;
  // TODO close log file

  return NULL;
}




static void *arduino_connect_thread(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;
  data->serial_path = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(data->serial_cmb));

  timestamp(data, "connecting to \"%s\"", data->serial_path);
  data->serial_fd = ard_openserial(data->serial_path);

  if (data->serial_fd < 0) {
    timestamp_error(data, "Failed to connect: (%d) %s", errno, strerror(errno));

    // re-enable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);

    // unref thread
    g_thread_unref(connect_thread);
    return NULL;
  }

  timestamp(data, "Waiting on Arduino...");
  wait_for(data, "ON", 100);
  timestamp(data, "Connected!");

  // enable disconnect button, disable all input fields while connection is active.
  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);

  wait_for(data, "WAIT", 100);

  timestamp(data, "Sending run parameters to Arduino");
  send_key_value_to_arduino(
      data, 
      "setpoint", 
      (void *)gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)),
      T_STR);

  send_key_value_to_arduino(
      data, 
      "kp", 
      (void *)gtk_entry_get_text(GTK_ENTRY(data->kp_inp)),
      T_STR);

  send_key_value_to_arduino(
      data, 
      "ki", 
      (void *)gtk_entry_get_text(GTK_ENTRY(data->ki_inp)),
      T_STR);

  send_key_value_to_arduino(
      data, 
      "kd", 
      (void *)gtk_entry_get_text(GTK_ENTRY(data->kd_inp)),
      T_STR);

  timestamp(data, "All parameters sent successfully!");

  log_thread = g_thread_new("log_thread", log_update_thread, data);

  return NULL;
}



void cb_connect(GObject *obj, struct Data *data)
{

  if (check_form(data))
    return;

  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  connect_thread = g_thread_new("connect_thread", arduino_connect_thread, data);

}




void cb_disconnect(GObject *obj, struct Data *data)
{

  LOG_STOPPED = 1;
  g_thread_exit(log_thread);
  g_thread_join(log_thread);

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
  
  timestamp(data, "Closing...");

  if (!LOG_STOPPED)
    cb_disconnect(NULL, data);

  gtk_main_quit();

}



void cb_refresh_serial(GObject *obj, struct Data *data )
{
  refresh_serial_list(data);
}




