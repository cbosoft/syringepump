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

  if (gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab))) {
    CHECK_ENTRY_NUMBER(data->dc_inp,
        "Duty cycle is a required field for this control scheme.", 
        "Duty cycle must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
  }
  else {
    CHECK_ENTRY_NUMBER(data->setpoint_inp, 
        "Set point is a required field for PID control.", 
        "Set point must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
    CHECK_ENTRY_NUMBER(data->kp_inp,
        "KP is a required field for PID control.",
        "KP must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->ki_inp,
        "KI is a required field for PID control.",
        "KI must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->kd_inp,
        "KD is a required field for PID control.",
        "KD must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
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


void form_set_cursor(struct Data *data, const char *name)
{
  GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(data->main_win));

  if (win == NULL)
    return;

  GdkDisplay *display = gdk_display_get_default();
  GdkCursor *curs = gdk_cursor_new_from_name(display, name);

  gdk_window_set_cursor(GDK_WINDOW(win), curs);
}


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




void form_set_sensitive(struct Data *data, int sensitivity_flag)
{
  switch (sensitivity_flag){
    case FORM_BUSY:
      // everything insensitive
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 0);
      form_set_cursor(data, "wait");
      break;
    case FORM_CONNECTED:
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 0);
      form_set_cursor(data, "normal");
      break;
    case FORM_DISCONNECTED:
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 1);
      form_set_cursor(data, "normal");
      break;
    case FORM_REFRESHING:
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 0);
      form_set_cursor(data, "wait");
      break;
    case FORM_NOSERIAL:
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 0);
      form_set_cursor(data, "normal");
      break;
    default:
      timestamp(NULL, 1, "Tried to set an unknown sensitivity");
      // fall-through
    case FORM_ALL:
      gtk_widget_set_sensitive(GTK_WIDGET(data->disconn_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->setpoint_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kp_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->ki_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->kd_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->dc_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->tag_inp), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(data->control_tab), 1);
      form_set_cursor(data, "normal");
      break;
  }
}
