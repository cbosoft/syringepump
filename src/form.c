#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>


#include "form.h"
#include "error.h"
#include "util.h"


#define IS_EMPTY_ENTRY(INP) (strlen(gtk_entry_get_text(GTK_ENTRY(INP))) == 0)
#define CHECK_ENTRY_NUMBER(DATA,MESG_EMPTY,MESG_NOT_NUM) \
  if (IS_EMPTY_ENTRY(DATA)) { \
      rv = 1; \
      timestamp_error(data, 1, MESG_EMPTY); \
    } \
    else { \
      if (is_not_number( gtk_entry_get_text(GTK_ENTRY(DATA)) )) { \
        rv = 1; \
        timestamp_error(data, 1, MESG_NOT_NUM); \
      } \
    }


int check_form(struct Data *data)
{
  int rv = 0;

  // check over the form, pre connection.
  // none of the inputs should be empty (setpoint, kp, ki, kd, tag)

  // setpoint, kp, ki, kd should be numbers
  // (enforced by gtk, but can be overridden using cli args, so do need to 
  // check here)

  // TODO: change to switch case
  int pageno = gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab));

  switch (pageno) {
    
  case PAGE_NO_CONTROL:
    CHECK_ENTRY_NUMBER(data->dc_inp,
        "Duty cycle is a required field for this control scheme.", 
        "Duty cycle must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
    break;
    
  case PAGE_PID_FLOW:
  case PAGE_PID_FORCE:
    CHECK_ENTRY_NUMBER(data->setpoint_inp_force, 
        "Set point is a required field for PID control.", 
        "Set point must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
    CHECK_ENTRY_NUMBER(data->kp_inp_force,
        "KP is a required field for PID control.",
        "KP must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->ki_inp_force,
        "KI is a required field for PID control.",
        "KI must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->kd_inp_force,
        "KD is a required field for PID control.",
        "KD must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    break;

  default:
    timestamp_error(data, 1,
        "Oops! Something went wrong. Please let chris know this happened!");
    break;
  }

  CHECK_ENTRY_NUMBER(data->buflen_inp,
      "Stop buffer length is a required field.",
      "Buffer length must be a number (containing only numbers 0-9 and decimal points "
      "('.').");
  CHECK_ENTRY_NUMBER(data->dia_inp,
      "Syringe diameter is a required field.",
      "Syringe diameter must be a number (containing only numbers 0-9 and decimal points "
      "('.').");

  if (IS_EMPTY_ENTRY(data->tag_inp)) {
    rv = 1;
    timestamp_error(data, 1,
        "Tag should not be empty; use it to describe the run in a few words.");
  }

  // if tag contains spaces, periods, or underscores; they will be 
  // replaced with dashes
  // this is handled in the generate tag function.

  return rv;
}




// Set cursor to a specific type
void form_set_cursor(struct Data *data, const char *name)
{
  GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(data->main_win));

  if (win == NULL)
    return;

  GdkDisplay *display = gdk_display_get_default();
  GdkCursor *curs = gdk_cursor_new_from_name(display, name);

  gdk_window_set_cursor(GDK_WINDOW(win), curs);
}




// Progress bar functions and data
struct progress_callback_data {
  struct Data *data;
  double fraction;
};

gboolean prog_pulse_callback(struct Data *data)
{
  gtk_progress_bar_pulse(GTK_PROGRESS_BAR(data->progress));
  return 0;
}

gboolean prog_set_callback(struct progress_callback_data *pcd)
{
  struct Data *data = pcd->data;
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data->progress), pcd->fraction);
  free(pcd);
  return 0;
}


void form_pulse_progress(struct Data *data)
{
  g_idle_add((GSourceFunc)prog_pulse_callback, data);
}


void form_set_progress(struct Data *data, double fraction)
{
  struct progress_callback_data *pcd = calloc(1, sizeof(struct progress_callback_data));
  pcd->data = data;
  pcd->fraction = fraction;
  g_idle_add((GSourceFunc)prog_set_callback, pcd);
}





// Depending on the situation, different parts of the form should be turned off or on
void form_set_sensitive(struct Data *data, int sensitivity_flag)
{
  // parts
  int 
    control = 0, 
    connection = 0,
    logging = 0, 
    connected = 0, 
    cursor_normal = 1;

  switch (sensitivity_flag){

  case FORM_ALL:
    control = 1;
    connection = 1;
    logging = 1;
    break;
      
  case FORM_BUSY:
  case FORM_REFRESHING:
    cursor_normal = 0;
    break;

  case FORM_CONNECTED:
    connected = 1;
    break;

  case FORM_DISCONNECTED:
    control = 1;
    connection = 1;
    logging = 1;
    break;

  case FORM_NOSERIAL:
    connection = 1;
    break;
      
  }

  // control
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp_force), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp_force), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp_force), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp_force), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), control);
  gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), control);

  // connection
  gtk_widget_set_sensitive(GTK_WIDGET(data->serial_cmb), connection);
  gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), connection);

  // logging
  gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_time_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_force_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_flow_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_ca_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_loadcell_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_ticks_chk), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(data->log_folder_fch), logging);

  gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), connected);
  gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), !connected);

  form_set_cursor(data, cursor_normal ? "normal" : "wait");
}
