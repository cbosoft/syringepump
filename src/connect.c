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
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  timestamp(data, 0, "connecting to \"%s\"", data->serial_path);

  int rv = open_serial(data->serial_path, data);
  if (rv < 0) {
    switch (rv) {
      case -1:
        timestamp_error(data, 0, "Failed to connect");
        break;
      case -2:
      case -3:
        timestamp_error(data, 0, "Failed to apply serial settings");
        break;
    }
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
    g_thread_unref(connect_worker_thread);
    return NULL;
  }

  timestamp(data, 0, "Waiting on Arduino...");
  switch (wait_for(data, 0, "ON", 100, &data->connect_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, "Arduino connection timed out!");
      return NULL;
  }

  timestamp(data, 0, "Connected!");

  // enable disconnect button, disable all input fields while connected.
  form_set_sensitive(data, FORM_CONNECTED);

  timestamp(data, 0, "Waiting on Arduino...");
  switch (wait_for(data, 0, "WAIT", 100, &data->connect_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, "Arduino connection timed out!");
      return NULL;
  }

  timestamp(data, 0, "Sending run parameters to Arduino");


  switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab))) {

    case 2: // NO CONTROL: DC ONLY
      send_data_packet(
        data, 
        0,
        "dc",
        gtk_entry_get_text(GTK_ENTRY(data->dc_inp)));
      break;

    case 1: // PID (FORCE) CONTROL
      send_data_packet(
        data, 
        0,
        "var", "force");

      send_data_packet(
        data, 
        0,
        "setpoint", 
        gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp_force)));

      send_data_packet(
        data, 
        0,
        "kp", 
        (void *)gtk_entry_get_text(GTK_ENTRY(data->kp_inp_force)));

      send_data_packet(
        data, 
        0,
        "ki", 
        gtk_entry_get_text(GTK_ENTRY(data->ki_inp_force)));

      send_data_packet(
        data, 
        0,
        "kd", 
        gtk_entry_get_text(GTK_ENTRY(data->kd_inp_force)));
      break;
    case 0:
      send_data_packet(
        data, 
        0,
        "var", "flow");
      send_data_packet(
        data, 
        0,
        "setpoint", 
        gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)));

      send_data_packet(
        data, 
        0,
        "kp", 
        (void *)gtk_entry_get_text(GTK_ENTRY(data->kp_inp)));

      send_data_packet(
        data, 
        0,
        "ki", 
        gtk_entry_get_text(GTK_ENTRY(data->ki_inp)));

      send_data_packet(
        data, 
        0,
        "kd", 
        gtk_entry_get_text(GTK_ENTRY(data->kd_inp)));
      break;
  }


  send_data_packet(
      data, 
      0,
      "bl", 
      gtk_entry_get_text(GTK_ENTRY(data->buflen_inp)));
  send_data_packet(
      data, 
      0,
      "di", 
      gtk_entry_get_text(GTK_ENTRY(data->dia_inp)));
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
