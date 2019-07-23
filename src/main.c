#include <gtk/gtk.h>
#include <pthread.h>

#include "error.h"
#include "serial.h"
#include "callbacks.h"

int LOG_STOPPED = 1;
pthread_t log_thread;

int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GError *error = NULL;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "gui/main2.ui", &error) == 0) {
    timestamp(NULL, "Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }


  struct connect_data *cd = create_cd(
      -1, 
      "/dev/ttyACM0",
      0,
      gtk_builder_get_object(builder, "winMain"),
      gtk_builder_get_object(builder, "btnConn"),
      gtk_builder_get_object(builder, "btnDisconn"),
      gtk_builder_get_object(builder, "btnSerialRefresh"),
      gtk_builder_get_object(builder, "lblLog"),
      gtk_builder_get_object(builder, "scroll"),
      gtk_builder_get_object(builder, "cmbSerial"));

  get_serial_name(GTK_COMBO_BOX_TEXT(cd->serial_cmb), GTK_WIDGET(cd->conn_btn));

  g_signal_connect(cd->main_win, "destroy", G_CALLBACK(cb_quit), cd);
  g_signal_connect(cd->conn_btn, "clicked", G_CALLBACK(cb_connect), cd);
  g_signal_connect(cd->disconn_btn, "clicked", G_CALLBACK(cb_disconnect), cd);
  g_signal_connect(cd->refresh_btn, "clicked", G_CALLBACK(cb_refresh_serial), cd);

  gtk_main();

  free(cd);

  return 0;
}
