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
  if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
    timestamp(NULL, "Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }


  struct Data *data = new_data(
      -1, 
      "/dev/ttyACM0",
      0,
      gtk_builder_get_object(builder, "winMain"),
      gtk_builder_get_object(builder, "btnConnect"),
      gtk_builder_get_object(builder, "btnDisconnect"),
      gtk_builder_get_object(builder, "btnSerialRefresh"),
      gtk_builder_get_object(builder, "inpSetPoint"),
      gtk_builder_get_object(builder, "inpKP"),
      gtk_builder_get_object(builder, "inpKI"),
      gtk_builder_get_object(builder, "inpKD"),
      gtk_builder_get_object(builder, "lblLog"),
      gtk_builder_get_object(builder, "scroll"),
      gtk_builder_get_object(builder, "cmbSerial"));


  g_signal_connect(data->main_win, "destroy", G_CALLBACK(cb_quit), data);
  g_signal_connect(data->conn_btn, "clicked", G_CALLBACK(cb_connect), data);
  g_signal_connect(data->disconn_btn, "clicked", G_CALLBACK(cb_disconnect), data);
  g_signal_connect(data->refresh_btn, "clicked", G_CALLBACK(cb_refresh_serial), data);

  timestamp(data, "Gui started");

  refresh_serial_list(data);

  gtk_main();

  free(data);

  return 0;
}
