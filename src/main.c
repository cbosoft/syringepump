#include <gtk/gtk.h>
#include <pthread.h>

#include "error.h"
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
    timestamp("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  GObject *main_win     = gtk_builder_get_object(builder, "winMain");
  GObject *log_lbl      = gtk_builder_get_object(builder, "lblLog");
  GObject *close_btn    = gtk_builder_get_object(builder, "btnClose");
  GObject *conn_btn     = gtk_builder_get_object(builder, "btnConn");
  GObject *disconn_btn  = gtk_builder_get_object(builder, "btnDisconn");
  GObject *ok_btn       = gtk_builder_get_object(builder, "btnOK");
  GObject *scroll       = gtk_builder_get_object(builder, "scroll");

  struct connect_data *cd = create_cd(
      -1, 
      "/dev/ttyACM0",
      0,
      main_win,
      conn_btn,
      disconn_btn,
      log_lbl,
      scroll);

  g_signal_connect(main_win, "destroy", G_CALLBACK(cb_quit), cd);
  g_signal_connect(close_btn, "clicked", G_CALLBACK(cb_quit), cd);
  g_signal_connect(ok_btn, "clicked", G_CALLBACK(cb_quit), cd);
  g_signal_connect(conn_btn, "clicked", G_CALLBACK(cb_connect), cd);
  g_signal_connect(disconn_btn, "clicked", G_CALLBACK(cb_disconnect), cd);

  gtk_main();

  free(cd);

  return 0;
}
