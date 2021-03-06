#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gtk/gtk.h>

#ifdef LINUX
#include <X11/Xlib.h>
#endif

#include "error.h"
#include "serial.h"
#include "callbacks.h"
#include "version.h"
#include "disconnect.h"
#include "refresh.h"
#include "threads.h"
#include "tuning_plotter.h"
#include "log.h"
#include "form.h"
#include "status.h"
#include "util.h"




struct Data *data;



#ifndef WINDOWS
void catch(int signal)
{
  switch (signal){
    default:
      disconnect(data, 1);
      break;
  }
  exit(1);
}
#endif




int main (int argc, char **argv)
{

  // if called from _not_ a terminal, redirect stderr to file
  if (!isatty(fileno(stdin)))
    if (!freopen("syringepump_error_log.txt", "a", stderr))
      freopen("syringepump_error_log.txt", "w", stderr);

#ifdef LINUX
  XInitThreads();
#endif

  gtk_init(&argc, &argv);

  timestamp(NULL, 1, "GTK initialised.");

  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;
  if (gtk_builder_add_from_file(builder, "/usr/share/syringepump/main.ui", &error) == 0) {
    timestamp(NULL, 1, "Error loading layout file: %s\n", error->message);
    g_clear_error(&error);
    if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
      timestamp(NULL, 1, "Error loading layout file: %s\n", error->message);
      g_clear_error(&error);
      return 1;
    }
    else {
      timestamp(NULL, 1, "Using local layout.");
    }
  }
  timestamp(NULL, 1, "Layout loaded.");


  data = calloc(1, sizeof(struct Data));

  // Serial
#ifdef WINDOWS
  data->serial_handle = NULL; // TODO
#else
  data->serial_fd = -1;
#endif
  data->serial_path = "/dev/ttyACM0";

  // Logging
  data->tag = NULL;
  data->logpath = NULL;
  data->log_worker_status = THREAD_NULL;
  data->refresh_worker_status = THREAD_NULL;
  data->connect_worker_status = THREAD_NULL;
  data->composition_data = NULL;

  data->builder = builder;
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(get_object_safe(data, "fchLogFolder")), getenv("HOME"));

  // SET UP
  gtk_window_set_title(GTK_WINDOW(get_object_safe(data, "winMain")), "Syringepump ("LONG_VERSION")");
  timestamp(data, 1, "Starting Syringepump (%s)", LONG_VERSION);

#ifndef WINDOWS
  // TODO error handling around this
  signal(SIGINT, catch);
#else
  // TODO signal catching or equivalent?
#endif

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--tag") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entTag")), argv[i]);
    }
  }

  // signals -->
  g_signal_connect(get_object_safe(data, "winMain"), "destroy", G_CALLBACK(cb_quit_clicked), data);
  g_signal_connect(get_object_safe(data, "btnConnect"), "clicked", G_CALLBACK(cb_begin_clicked), data);
  g_signal_connect(get_object_safe(data, "btnDisconnect"), "clicked", G_CALLBACK(cb_stop_clicked), data);
  g_signal_connect(get_object_safe(data, "btnSerialRefresh"), "clicked", G_CALLBACK(cb_refresh_clicked), data);
  g_signal_connect(get_object_safe(data, "entTag"), "changed", G_CALLBACK(cb_tag_text_changed), data);

  g_signal_connect(get_object_safe(data, "cmbSetpointFunction"), "changed", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "cmbControlMethod"), "changed", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "cmbControlledVariable"), "changed", G_CALLBACK(cb_setter_radio_changed), data);

  g_signal_connect(get_object_safe(data, "radManual"), "toggled", G_CALLBACK(cb_pid_manual_radio_changed), data);
  g_signal_connect(get_object_safe(data, "fcbCompTuning"), "file-set", G_CALLBACK(cb_file_set), data);
  // <--

  read_tuning_data("/usr/share/syringepump/default_tuning.csv", &data->composition_data);

  if (!data->composition_data) {
    timestamp(NULL, 0, "Error reading default tuning data: trying local");
    read_tuning_data("default_tuning.csv", &data->composition_data);
    if (!data->composition_data) {
      timestamp(NULL, 0, "Error reading local runing data.");
      exit(1);
    }
  }

  form_setter_update(data);
  get_new_log_name(data, NULL);
  init_tuning_plot(data);
  status_init(get_object_safe(data, "drawStatusPlot"));

  ptble_usleep(100000);
  refresh(data);
  ptble_usleep(100000); // 100ms

  timestamp(data, 1, "GUI started");
  gtk_main();
  timestamp(data, 1, "Freeing memory...");

  status_free();
  composition_data_free(data->composition_data);
  free_tuning_plot();

  if (data->logpath)
    free(data->logpath);

  if (data->tag)
    free(data->tag);

  free(data);

  timestamp(data, 1, "Done!");
  return 0;
}
