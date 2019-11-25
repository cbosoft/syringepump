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

  const char *kp = gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKP")));
  const char *ki = gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKI")));
  const char *kd = gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKD")));
  int response = gtk_dialog_run(GTK_DIALOG(get_object_safe(data, "winTuningDialog")));

  switch (response) {

    case GTK_RESPONSE_CLOSE:
    case GTK_RESPONSE_CANCEL:
      gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKP")), kp);
      gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKI")), ki);
      gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKD")), kd);
      break;

  }

  gtk_widget_hide(GTK_WIDGET(get_object_safe(data, "winTuningDialog")));
}
