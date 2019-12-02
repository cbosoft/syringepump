#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "error.h"
#include "log.h"
#include "serial.h"
#include "form.h"
#include "connect.h"
#include "disconnect.h"
#include "refresh.h"




void cb_lbl_size_changed(GObject *obj, GdkRectangle *allocation, struct Data *data)
{
  (void) obj;
  (void) allocation;

  GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(
      GTK_SCROLLED_WINDOW(get_object_safe(data, "scroll")));

  gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}




void cb_begin_clicked(GObject *obj, struct Data *data)
{
  (void) obj;

  connect_to(data);
}




void cb_stop_clicked(GObject *obj, struct Data *data)
{
  (void) obj;

  disconnect(data, 1);
}




void cb_quit_clicked(GObject *obj, struct Data *data)
{
  (void) obj;

  disconnect(data, 1);
  timestamp(data, 1, "Closing...");
  gtk_main_quit();

}



void cb_refresh_clicked(GObject *obj, struct Data *data )
{
  (void) obj;
  refresh(data);
}




void cb_tag_text_changed(GObject *obj, struct Data *data)
{
  (void) obj;
  get_new_log_name(data, NULL);
}




void cb_setter_radio_changed(GObject *obj, struct Data *data)
{
  (void) obj;
  form_setter_update(data);
}




//
void cb_tuning_clicked(GObject *obj, struct Data *data)
{
  (void) obj;

  int control_type = form_get_control_type(data), response = 1;
  char *kp = NULL;
  char *ki = NULL;
  char *kd = NULL;
  char *mt = NULL;

  switch (control_type) {
    case FORM_CONTROL_NONE:
      timestamp_error(data, 0, "this should never happen.");
      return;

    case FORM_CONTROL_PID:
      kp = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKP"))));
      ki = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKI"))));
      kd = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKD"))));
      response = gtk_dialog_run(GTK_DIALOG(get_object_safe(data, "winTuningDialog")));
      if (response != GTK_RESPONSE_ACCEPT) {
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKP")), kp);
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKI")), ki);
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKD")), kd);
      }
      gtk_widget_hide(GTK_WIDGET(get_object_safe(data, "winTuningDialog")));
      break;

    case FORM_CONTROL_MEAS:
      mt = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entMeasTime"))));
      response = gtk_dialog_run(GTK_DIALOG(get_object_safe(data, "winPassiveSettings")));
      if (response != GTK_RESPONSE_ACCEPT) {
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entMeasTime")), mt);
      }
      gtk_widget_hide(GTK_WIDGET(get_object_safe(data, "winPassiveSettings")));
      break;
  }



}
