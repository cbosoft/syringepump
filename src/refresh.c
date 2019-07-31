#include "refresh.h"
#include "threads.h"
#include "error.h"
#include "form.h"
#include "errno.h"



extern int refresh_worker_status;
static GThread *refresh_worker_thread = NULL;



// resource temporarily unavailable thrown here
static void *refresh_worker(void *vptr_data)
{
  refresh_worker_status = THREAD_STARTED;
  struct Data *data = (struct Data *)vptr_data;
  form_set_sensitive(data, FORM_REFRESHING);

  timestamp(data, 0, "Searching for Arduino...");

  struct timespec ms300_span;
  ms300_span.tv_sec = 0;
  ms300_span.tv_nsec = 300*1000*1000;
  nanosleep(&ms300_span, NULL);

  gtk_combo_box_text_remove_all(
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  const char *dev = "/dev/*";

  if (d == NULL) {

    g_thread_unref(refresh_worker_thread);
    refresh_worker_status = THREAD_NULL;
    timestamp_error(data, 0, "No Arduino found!");
    form_set_sensitive(data, FORM_NOSERIAL);
    return NULL;
  }


  int count = 0;

  while ((dir = readdir(d)) != NULL) {

    if (refresh_worker_status > THREAD_STARTED) {
      refresh_worker_status = THREAD_NULL;
      form_set_sensitive(data, FORM_NOSERIAL);
      return NULL;
    }

    if (strstr(dir->d_name, "ttyACM") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(
          GTK_COMBO_BOX_TEXT(data->serial_cmb), 
          path);

      count ++;

    }
  }

  if (errno) {
    timestamp_error(data, "Error reading contents of %s", dev);
  }

  if (!count) {
    timestamp_error(data, "No Arduino found!");
    form_set_sensitive(data, FORM_NOSERIAL);
    refresh_worker_status = THREAD_NULL;
    return NULL;
  }

  form_set_sensitive(data, FORM_DISCONNECTED);
  refresh_worker_status = THREAD_NULL;

  gtk_combo_box_set_active(GTK_COMBO_BOX(data->serial_cmb), 0);

  if (count == 1) {
    timestamp(data, "Arduino found!");
  }
  else {
    timestamp(data, "Multiple possible Arduino found.");
  }

  return NULL;
}




void refresh(struct Data* data)
{
  refresh_worker_thread = g_thread_new(
      "refresh_thread", 
      refresh_worker, 
      data);
}


void cancel_refresh(struct Data *data)
{
  if (refresh_worker_status < THREAD_CANCELLED) {
    refresh_worker_status = THREAD_CANCELLED;
    g_thread_join(refresh_worker_thread);
  }
}
