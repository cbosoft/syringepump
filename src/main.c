#include <gtk/gtk.h>

#ifdef LINUX
#include <X11/Xlib.h>
#endif

#include "error.h"
#include "serial.h"
#include "callbacks.h"

int LOG_STOPPED = 1;

int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GError *error = NULL;
  
#ifdef LINUX
  XInitThreads();
#endif

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
    timestamp(NULL, "Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }


  struct Data *data = malloc(sizeof(struct Data));;

  // Serial
  data->serial_fd = -1;
  data->serial_path = "/dev/ttyACM0";

  // ??
  data->res = 0;

  // Logging
  data->tag = NULL;
  data->logpath = NULL;

  // GUI :: windows
  data->main_win = gtk_builder_get_object(builder, "winMain");

  // GUI :: buttons
  data->conn_btn = gtk_builder_get_object(builder, "btnConnect");
  data->disconn_btn = gtk_builder_get_object(builder, "btnDisconnect");
  data->refresh_btn = gtk_builder_get_object(builder, "btnSerialRefresh");

  // GUI :: inputs
  data->setpoint_inp = gtk_builder_get_object(builder, "inpSetPoint");
  data->kp_inp = gtk_builder_get_object(builder, "inpKP");
  data->ki_inp = gtk_builder_get_object(builder, "inpKI");
  data->kd_inp = gtk_builder_get_object(builder, "inpKD");
  data->tag_inp = gtk_builder_get_object(builder, "inpTag");
  data->log_folder_fch = gtk_builder_get_object(builder, "fchLogFolder");
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(data->log_folder_fch), getenv("HOME"));

  // GUI :: other
  data->log_lbl = gtk_builder_get_object(builder, "lblLog");
  data->scroll = gtk_builder_get_object(builder, "scroll");
  data->serial_cmb = gtk_builder_get_object(builder, "cmbSerial");

  g_signal_connect(data->main_win, "destroy", G_CALLBACK(cb_quit), data);
  g_signal_connect(data->conn_btn, "clicked", G_CALLBACK(cb_connect), data);
  g_signal_connect(data->disconn_btn, "clicked", G_CALLBACK(cb_disconnect), data);
  g_signal_connect(data->refresh_btn, "clicked", G_CALLBACK(cb_refresh_serial), data);
  g_signal_connect(data->log_lbl, "size-allocate", G_CALLBACK(cb_lbl_size_changed), data);

  timestamp(data, "Gui started");

  cb_refresh_serial(NULL, data);

  gtk_main();
  
  if (data->logpath != NULL)
    free(data->logpath);
  free(data);

  return 0;
}
