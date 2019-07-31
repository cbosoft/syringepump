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
  GdkDisplay *display = gdk_display_get_default();
  GdkCursor *curs = gdk_cursor_new_from_name(display, name);
  GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(data->main_win));
  gdk_window_set_cursor(GDK_WINDOW(win), curs);
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
