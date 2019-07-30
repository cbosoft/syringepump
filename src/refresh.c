#include "refresh.h"
#include "threads.h"
#include "error.h"



extern int refresh_worker_status;
static GThread *refresh_worker_thread;




// TODO use new status variable
static void *refresh_worker(void *vptr_data)
{
  refresh_worker_status = THREAD_STARTED;
  struct Data *data = (struct Data *)vptr_data;

  gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 0);

  timestamp(data, "Searching for Arduino...");
  struct timespec ms300_span;
  ms300_span.tv_sec = 0;
  ms300_span.tv_nsec = 300*1000*1000;
  nanosleep(&ms300_span, NULL);

  gtk_combo_box_text_remove_all(
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  if (!d) {
    timestamp_error(data, "Error reading /dev/*");

    g_thread_unref(refresh_worker_thread);
    return NULL;
  }

  int count = 0;

  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "ttyACM") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(
          GTK_COMBO_BOX_TEXT(data->serial_cmb), 
          path);

      count ++;

    }
  }

  if (!count) {
    timestamp_error(data, "No Arduino found!");

    // disable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  }
  else {

    // enable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(data->serial_cmb), 0);
    if (count == 1) {
      timestamp(data, "Arduino found!");
    }
    else {
      timestamp(data, "Multiple possible Arduino found.");
    }
  }

  gtk_widget_set_sensitive(GTK_WIDGET(data->refresh_btn), 1);

  // g_thread_unref(refresh_thread);
  return NULL;
}




void refresh(struct Data* data)
{
  refresh_worker_thread = g_thread_new(
      "refresh_thread", 
      refresh_worker, 
      data);
}
