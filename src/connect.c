#include <stdlib.h>

#include "connect.h"
#include "error.h"
#include "serial.h"
#include "threads.h"
#include "form.h"
#include "log.h"




static GThread *connect_worker_thread;



static void *connect_worker(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;
  data->connect_worker_status = THREAD_STARTED;
  data->serial_path = gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(get_object_safe(data, "cmbSerial")));

  timestamp(data, 0, "connecting to \"%s\"", data->serial_path);

  int rv = open_serial(data->serial_path, data);
  if (rv < 0) {
    switch (rv) {
      case -1:
        timestamp_error(data, 0, 1, "Failed to connect");
        break;
      case -2:
      case -3:
        timestamp_error(data, 0, 1, "Failed to apply serial settings");
        break;
    }
    gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnConnect")), 1);
    g_thread_unref(connect_worker_thread);
    return NULL;
  }

  timestamp(data, 0, "Waiting on Arduino...");
  switch (wait_for(data, 0, "ON", 100, &data->connect_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, 0, "Arduino connection timed out!");
      return NULL;
  }

  timestamp(data, 0, "Connected!");

  // enable disconnect button, disable all input fields while connected.
  form_set_sensitive(data, FORM_CONNECTED);

  timestamp(data, 0, "Waiting on Arduino...");
  switch (wait_for(data, 0, "WAIT", 100, &data->connect_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, 0, "Arduino connection timed out!");
      return NULL;
  }

  timestamp(data, 0, "Sending run parameters to Arduino");

  char *control_packet = NULL;
  switch (form_get_control_type(data)) {

    case FORM_CONTROL_ERROR:
      // don't need to handle error: should be checked earlier
    case FORM_CONTROL_NONE:
      // control "None" doesn't require params to be sent
      break;

    case FORM_CONTROL_MEAS:
    case FORM_CONTROL_PID:
      control_packet = form_get_controller_packet(data);
      send_data_packet(data, 0, "TP", control_packet);
      free(control_packet);
      break;
  }


  char *setter_packet = form_get_setter_packet(data);
  send_data_packet(data, 0, "SP", setter_packet);
  free(setter_packet);

  char *bldi_packet = form_get_bldi_packet(data);
  send_data_packet(data, 0, "BD", bldi_packet);
  free(bldi_packet);

  timestamp(data, 0, "All parameters sent successfully!");
  start_log(data);
  return NULL;
}




void connect_to(struct Data *data)
{

  if (check_form(data))
    return;

  puts("AFTER CHECK FORM");

  form_set_sensitive(data, FORM_BUSY);

  connect_worker_thread = g_thread_new(
      "connect_thread", 
      connect_worker, 
      data);
}



int cancel_connect(struct Data *data)
{

  if (data->connect_worker_status < THREAD_CANCELLED && data->connect_worker_status > THREAD_NULL) {
    data->connect_worker_status = THREAD_CANCELLED;
    g_thread_join(connect_worker_thread);
    return 1;
  }

  return 0;
}
