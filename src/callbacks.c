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



void cb_pid_manual_radio_changed(GObject *obj, struct Data *data)
{
  (void) obj;
  int manual = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj));
  GObject *kp_ent = get_object_safe(data, "entKP");
  GObject *ki_ent = get_object_safe(data, "entKI");
  GObject *kd_ent = get_object_safe(data, "entKD");
  GObject *filechooser = get_object_safe(data, "fcbCompTuning");

  gtk_widget_set_sensitive(GTK_WIDGET(kp_ent), manual);
  gtk_widget_set_sensitive(GTK_WIDGET(ki_ent), manual);
  gtk_widget_set_sensitive(GTK_WIDGET(kd_ent), manual);
  gtk_widget_set_sensitive(GTK_WIDGET(filechooser), !manual);
}
