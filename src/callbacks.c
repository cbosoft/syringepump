#include <errno.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "ardiop.h"
#include "error.h"
#include "log.h"
#include "serial.h"

extern int LOG_STOPPED;

static GThread *log_thread;
static GThread *connect_thread;
static GThread *refresh_thread;




static void *refresh_serial_list(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;

  timestamp(data, "Searching for Arduino...");
  struct timespec ms300_span;
  ms300_span.tv_sec = 0;
  ms300_span.tv_nsec = 300*1000*1000;
  nanosleep(&ms300_span, NULL);

  gtk_combo_box_text_remove_all(
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  if (!d) {
    timestamp_error(data, "Error reading /dev/*");

    g_thread_unref(refresh_thread);
    return NULL;
  }

  int count = 0;

  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "ttyACM") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(
          GTK_COMBO_BOX_TEXT(data->serial_cmb), 
          path);

      count ++;

    }
  }

  if (!count) {
    timestamp_error(data, "No Arduino found!");

    // disable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  }
  else {

    // enable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(data->serial_cmb), 0);
    if (count == 1) {
      timestamp(data, "Arduino found!");
    }
    else {
      timestamp(data, "Multiple possible Arduino found.");
    }
  }

  gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 1);

  // g_thread_unref(refresh_thread);
  return NULL;
}




static void *log_update_loop(void *void_data)
{ 
  struct Data *data = (struct Data *)void_data;
  const int print_every = 100;
  
  timestamp(data, "Waiting for Arduino...");
  wait_for(data, "START", 100);
  timestamp(data, "Arduino ready, starting!");

  get_new_log_name(data);
  FILE *fp = fopen(data->logpath, "w");

  int lineno = 0, charno = 0, timeout = 1000;
  LOG_STOPPED = 0;
  
  struct timespec ms_span;
  ms_span.tv_sec = 0;
  ms_span.tv_nsec = 1000*1000;

  while (!LOG_STOPPED) {
    char received_text[512];

    for (int i = 0; i < 512; i++)
      received_text[i] = 0;
    
    char b[1];
    charno = 0;
    do {
      int n = read(data->serial_fd, b, 1);

      if(n == -1) {
        // error
        timestamp_error(NULL, 
            "something went wrong reading a byte (read failed) (%d) %s", 
            errno, strerror(errno));
        exit(0); // TODO handle properly
      }
      else if(n == 0) {
        // no read
        nanosleep(&ms_span, NULL);  // wait 1 msec try again
        timeout--;
        if(timeout == 0){
          timestamp_error(NULL, 
              "something went wrong reading a byte (timed out) (%d), %s", 
              errno, strerror(errno));
          exit(0); // TODO handle properly
        }

        continue;
      }

      received_text[charno] = b[0];

      charno++;

    } while(b[0] != '\n' && charno < 512 && !LOG_STOPPED);

    received_text[charno-1] = '0';

    if (received_text[0] == 'P') {
      timestamp(NULL, " :: %s", received_text);
    }
    else {
      timestamp( 
          ((lineno % print_every) == 0) ? data : NULL,
          "R: %s",
          received_text);
      lineno = 
        (lineno % print_every == 0) ? (1) : (lineno + 1);
    }

    if (strcmp(received_text, "STOP") == 0) {
      // arduino requests stop
      timestamp(data, "Arduino finished!");
      LOG_STOPPED = 1;
    }
    else {
      // write to log file
      fprintf(fp, "%s\n", received_text);
    }

  }

  fclose(fp);

  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 1);

  close(data->serial_fd);

  //g_thread_unref(log_thread);
  return NULL;
}




static void *arduino_connect_thread(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;
  data->serial_path = gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  timestamp(data, "connecting to \"%s\"", data->serial_path);

  int rv = ard_openserial(data->serial_path, &data->serial_fd);
  if (rv < 0) {
    switch (rv) {
      case -1:
        timestamp_error(data, "Failed to connect");
        break;
      case -2:
      case -3:
        timestamp_error(data, "Failed to apply serial settings");
        break;
    }
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
    g_thread_unref(connect_thread);
    return NULL;
  }

  timestamp(data, "Waiting on Arduino...");
  wait_for(data, "ON", 100);
  timestamp(data, "Connected!");

  // enable disconnect button, disable all input fields while connected.
  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);

  timestamp(data, "Waiting for Arduino...");
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

  log_thread = g_thread_new("log_thread", log_update_loop, data);

  return NULL;
}








///////////////////////////////////////////////////////////////////////////////
// BUTTON CALLBACKS ///////////////////////////////////////////////////////////

void cb_lbl_size_changed(GObject *obj, GdkRectangle *allocation, struct Data *data)
{
  GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(data->scroll));

  gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}


void cb_connect(GObject *obj, struct Data *data)
{

  if (check_form(data))
    return;

  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);

  connect_thread = g_thread_new(
      "connect_thread", 
      arduino_connect_thread, 
      data);

}




void cb_disconnect(GObject *obj, struct Data *data)
{

  LOG_STOPPED = 1;
  g_thread_join(log_thread);

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
  gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 0);

  connect_thread = g_thread_new(
      "refresh_thread", 
      refresh_serial_list, 
      data);
}




