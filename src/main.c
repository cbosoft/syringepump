#include <stdlib.h>
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



int log_worker_status = THREAD_NULL;
int refresh_worker_status = THREAD_NULL;
int connect_worker_status = THREAD_NULL;
struct Data *data;



#ifndef WINDOWS
void catch(int signal)
{
  switch (signal){
    default:
      disconnect(data);
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




int main (int argc, char **argv)
{
  GtkBuilder *builder;
  GError *error = NULL;
  
#ifdef LINUX
  XInitThreads();
#endif

  gtk_init(&argc, &argv);

  // preliminary arg check
  for (int i = 0; i < argc; i++) {
    if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      usage();
      exit(0);
    }
  }

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "gui/main.ui", &error) == 0) {
    timestamp(NULL, "Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }


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

  // GUI :: windows
  data->main_win = gtk_builder_get_object(builder, "winMain");

  // GUI :: buttons
  data->conn_btn = gtk_builder_get_object(builder, "btnConnect");
  data->disconn_btn = gtk_builder_get_object(builder, "btnDisconnect");
  data->refresh_btn = gtk_builder_get_object(builder, "btnSerialRefresh");

  // GUI :: inputs
  data->setpoint_inp = gtk_builder_get_object(builder, "inpSetPoint");
  data->dc_inp = gtk_builder_get_object(builder, "inpDC");
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
  data->control_tab = gtk_builder_get_object(builder, "tabControl");

  // SET UP
  gtk_window_set_title(GTK_WINDOW(data->main_win), "Syringepump ("LONG_VERSION")");
  timestamp(data, "Starting Syringepump (%s)", LONG_VERSION);
  
#ifndef WINDOWS
  // TODO error handling around this
  signal(SIGINT, catch);
#else
  // TODO signal catching or equivalent?
#endif

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--set-point") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->setpoint_inp), argv[i]);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(data->control_tab), 0);
    }
    if (strcmp(argv[i], "--kp") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->kp_inp), argv[i]);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(data->control_tab), 0);
    }
    if (strcmp(argv[i], "--ki") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->ki_inp), argv[i]);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(data->control_tab), 0);
    }
    if (strcmp(argv[i], "--kd") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->kd_inp), argv[i]);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(data->control_tab), 0);
    }
    if (strcmp(argv[i], "--dc") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->dc_inp), argv[i]);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(data->control_tab), 1);
    }
    if (strcmp(argv[i], "--tag") == 0) {
      i++;
      gtk_entry_set_text(GTK_ENTRY(data->tag_inp), argv[i]);
    }
  }

  g_signal_connect(data->main_win, "destroy", G_CALLBACK(cb_quit_clicked), data);
  g_signal_connect(data->conn_btn, "clicked", G_CALLBACK(cb_begin_clicked), data);
  g_signal_connect(data->disconn_btn, "clicked", G_CALLBACK(cb_stop_clicked), data);
  g_signal_connect(data->refresh_btn, "clicked", G_CALLBACK(cb_refresh_clicked), data);
  g_signal_connect(data->log_lbl, "size-allocate", G_CALLBACK(cb_lbl_size_changed), data);

  timestamp(data, "Gui started");

  refresh(data);

  gtk_main();
  
  if (data->logpath != NULL)
    free(data->logpath);
  free(data);

  return 0;
}
