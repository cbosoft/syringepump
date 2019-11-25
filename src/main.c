#include <stdlib.h>
#include <string.h>

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
#include "log.h"
#include "form.h"




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




void usage()
{
  fprintf(stderr, 
      "\n"
      "  Syringepump ("LONG_VERSION")\n"
      "\n"
      "  Syringe pump control software.\n"
      "\n"
      "          └────┰──────────┐\n"
      "       →   ────┫          ├──────\n"
      "          ┌────┸──────────┘\n"
      "\n"
         ////////////////////////////////////////////////////////////////////////////////
      "    An Arduino is used to control the speed and logging of data from a syringe\n"
      "    pump. The GUI should be fairly self explanatory, and I don't want to write\n"
      "    too much here as it may go out of date fairly quickly. Log an issue on \n"
      "    github (https://github.com/cbosoft/syringepump) if there's something wrong,\n"
      "    or if you want to request anything.\n"
      "\n"
      "  Command line\n"
      "\n"
      "    Control settings can be pre-loaded from the command line. Use the arguments as\n"
      "    below to pre-fill the form on the GUI.\n"
      "\n"
      "    syringepump [--set-point <set point>] \n"
      "                [--kp <kp>] \n"
      "                [--ki <ki>] \n"
      "                [--kd <kd>] \n"
      "                [--dc <dc>] \n"
      "                [--tag <tag>]"
      "\n"
      "    Square brackets indicate optional arguments (they all are). You can use as many\n"
      "    as few as you like. If a setting is not set on the commandline, the form will be\n"
      "    filled out with the default value. If the same option is written multiple times,\n"
      "    the last value is the one that will be used.\n"
      "\n");
}

GObject * get_object_safe(GtkBuilder *builder, const char *name)
{
  GObject *rv = gtk_builder_get_object(builder, name);

  if (rv == NULL) {
    timestamp(NULL, 0, "GTK object not found in builder \"%s\"", name);
    timestamp(NULL, 0, "Exiting...");
    exit(1);
  }

  return rv;
}




int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GError *error = NULL;
  
#ifdef LINUX
  XInitThreads();
#endif


  gtk_init(&argc, &argv);

  timestamp(NULL, 1, "GTK initialised.");
  char *cal = "0";

  // preliminary arg check
  for (int i = 0; i < argc; i++) {
    if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      usage();
      exit(0);
    }
    else if ((strcmp(argv[i], "--calibration") == 0) || (strcmp(argv[i], "-c") == 0)) {
      cal = "1";
    }
  }

  builder = gtk_builder_new();
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

  // ??
  data->res = 0;

  // Logging
  data->tag = NULL;
  data->logpath = NULL;
  data->log_worker_status = THREAD_NULL;
  data->refresh_worker_status = THREAD_NULL;
  data->connect_worker_status = THREAD_NULL;

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

  g_signal_connect(get_object_safe(data, "winMain"), "destroy", G_CALLBACK(cb_quit_clicked), data);
  g_signal_connect(get_object_safe(data, "btnConnect"), "clicked", G_CALLBACK(cb_begin_clicked), data);
  g_signal_connect(get_object_safe(data, "btnDisconnect"), "clicked", G_CALLBACK(cb_stop_clicked), data);
  g_signal_connect(get_object_safe(data, "btnSerialRefresh"), "clicked", G_CALLBACK(cb_refresh_clicked), data);
  g_signal_connect(get_object_safe(data, "lblLog"), "size-allocate", G_CALLBACK(cb_lbl_size_changed), data);
  g_signal_connect(get_object_safe(data, "entTag"), "changed", G_CALLBACK(cb_tag_text_changed), data);
  g_signal_connect(get_object_safe(data, "btnTuning"), "clicked", G_CALLBACK(cb_tuning_clicked), data);

  g_signal_connect(get_object_safe(data, "radConst"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "radRamp"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "radSine"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "radFlowControl"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "radPID"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  g_signal_connect(get_object_safe(data, "radNoControl"), "toggled", G_CALLBACK(cb_setter_radio_changed), data);
  form_setter_update(data);

  timestamp(data, 1, "GUI started");

  get_new_log_name(data, NULL);
  refresh(data);

  gtk_main();
  
  if (data->logpath != NULL)
    free(data->logpath);
  free(data);

  return 0;
}
