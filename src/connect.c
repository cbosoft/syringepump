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


  int pageno = gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab));
  char controlled_var, *setpoint = NULL, *tuning = NULL;
  const char *txtbx = NULL;
  switch (pageno) {

    case PAGE_NO_CONTROL:
      send_data_packet(
        data, 
        0,
        "DC",
        gtk_entry_get_text(GTK_ENTRY(data->dc_inp)));
      break;

    case PAGE_PID_FLOW:
    case PAGE_PID_FORCE:

      controlled_var = pageno == PAGE_PID_FORCE ? 'F' : 'Q';

      txtbx = pageno == PAGE_PID_FORCE ? gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp_force)) : gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp));
      setpoint = malloc((strlen(txtbx) + 3)*sizeof(char));
      setpoint[0] = controlled_var;
      setpoint[1] = 'C';
      setpoint[2] = '\0';
      // TODO: setpoint setter func
      strcat(setpoint, txtbx);
      send_data_packet( data, 0, "SP", setpoint);
      free(setpoint);

      tuning = malloc(60*sizeof(char));
      tuning[0] = '\0';
      strcat(tuning, gtk_entry_get_text(GTK_ENTRY(data->kp_inp_force)));
      strcat(tuning, ",");
      strcat(tuning, gtk_entry_get_text(GTK_ENTRY(data->ki_inp_force)));
      strcat(tuning, ",");
      strcat(tuning, gtk_entry_get_text(GTK_ENTRY(data->kd_inp_force)));

      send_data_packet(
        data, 
        0,
        "TP", 
        tuning);
      free(tuning);
      break;
  }


  send_data_packet(
      data, 
      0,
      "BF", 
      gtk_entry_get_text(GTK_ENTRY(data->buflen_inp)));
  send_data_packet(
      data, 
      0,
      "DI", 
      gtk_entry_get_text(GTK_ENTRY(data->dia_inp)));


  int log_time = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_time_chk)), 
      log_force = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_force_chk)), 
      log_flow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_flow_chk)), 
      log_ca = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_ca_chk)), 
      log_loadcell = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_loadcell_chk)), 
      log_ticks = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->log_ticks_chk));

  char *log_options = malloc(100*sizeof(char));
  sprintf(log_options, "%d", (log_time << 5) + (log_force << 4) + (log_flow << 3) + (log_ca << 2) + (log_loadcell << 1) + log_ticks);

  send_data_packet(
      data, 
      0,
      "LO", 
      log_options);

  // TODO send infor about what user wants logged
  timestamp(data, 0, "All parameters sent successfully!");

  free(log_options);
  
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
