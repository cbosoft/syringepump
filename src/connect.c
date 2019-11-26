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
        timestamp_error(data, 0, "Failed to connect");
        break;
      case -2:
      case -3:
        timestamp_error(data, 0, "Failed to apply serial settings");
        break;
    }
    gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnConnect")), 1);
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

  int 
    controlled_var = form_get_controlled_var(data), 
    controller_type = form_get_control_type(data), 
    setpoint_type = form_get_setter_type(data);

  char *setpoint = NULL, controlled_var_ch, *setter_params, setter_ch;

  // get character representing controlled var
  if (controller_type != FORM_CONTROL_NONE) {
    if (controlled_var == FORM_VAR_FLOW) {
      controlled_var_ch = 'Q'; // Flowrate
    }
    else {
      controlled_var_ch = 'F'; // Force
    }
  }
  else {
    controlled_var_ch = 'D'; // DC
  }

  setpoint = calloc(70, sizeof(char));

  switch (setpoint_type) {

    case FORM_SETTER_CONSTANT:
      setter_ch = 'C';
      setter_params = form_get_const_setter_params(data);
      break;

    case FORM_SETTER_RAMP:
      setter_ch = 'R';
      setter_params = form_get_ramp_setter_params(data);
      break;

    case FORM_SETTER_STEP:
      setter_ch = 'T';
      setter_params = form_get_step_setter_params(data);
      break;

    case FORM_SETTER_SINE:
      setter_ch = 'S';
      setter_params = form_get_sine_setter_params(data);
      break;

  }

  sprintf(setpoint, "%c%c%s", controlled_var_ch, setter_ch, setter_params);
  if (setpoint_type != FORM_SETTER_CONSTANT) {
    free(setter_params);
  }
  send_data_packet(data, 0, "SP", setpoint);
  free(setpoint);

  if (controller_type == FORM_CONTROL_PID) {
    char *pid_tuning = form_get_pid_params(data);
    send_data_packet(data, 0, "TP", pid_tuning);
    free(pid_tuning);
  }

  char *bldi_data = form_get_bldi_data(data);
  send_data_packet(data, 0, "BD", bldi_data);
  free(bldi_data);

  char *log_options = malloc(70*sizeof(char));
  sprintf(log_options, "%d", form_get_log_options(data));
  send_data_packet(data, 0, "LO", log_options);
  free(log_options);

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
