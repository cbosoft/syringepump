#include <gtk/gtk.h>
#include <glob.h>

#include "refresh.h"
#include "threads.h"
#include "error.h"
#include "form.h"
#include "errno.h"



static GThread *refresh_worker_thread = NULL;
static int number_failed_attempts = 0;



static void *refresh_worker(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;

  data->refresh_worker_status = THREAD_STARTED;

  // TODO do from callback
  form_set_sensitive(data, FORM_REFRESHING);

  timestamp(data, 0, "Searching for Arduino...");

  struct timespec ms300_span;
  ms300_span.tv_sec = 0;
  ms300_span.tv_nsec = 300*1000*1000;
  nanosleep(&ms300_span, NULL);

  // TODO do from callback
  gtk_combo_box_text_remove_all(
      GTK_COMBO_BOX_TEXT(data->serial_cmb));

  const char *dev = "/dev/ttyACM*";
  glob_t glob_res = {0};
  glob(dev, 0, NULL, &glob_res);
  errno = 0; // clear accumulated access errors

  if (glob_res.gl_pathc == 0) {
    number_failed_attempts ++;

    if (number_failed_attempts > 3) {
      timestamp_error(data, 0, "No Arduino found! If its still not showing up, try rebooting.");
    }
    else {
      timestamp_error(data, 0, "No Arduino found! Is it definitely plugged in?");
    }

    form_set_sensitive(data, FORM_NOSERIAL);
    data->refresh_worker_status = THREAD_STOPPED;
    return NULL;
  }

  for (int i = 0; i < (int)glob_res.gl_pathc; i++) {
    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(data->serial_cmb), 
        glob_res.gl_pathv[i]);
  }
  
  if (glob_res.gl_pathc == 1) {
    timestamp(data, 0, "Arduino found!");
  }
  else {
    timestamp(data, 0, "Multiple possible Arduino found.");
  }

  form_set_sensitive(data, FORM_DISCONNECTED);
  data->refresh_worker_status = THREAD_STOPPED;

  // TODO do from callback
  gtk_combo_box_set_active(GTK_COMBO_BOX(data->serial_cmb), 0);

  return NULL;
}




void refresh(struct Data* data)
{
  refresh_worker_thread = g_thread_new(
      "refresh_thread", 
      refresh_worker, 
      data);
}




int cancel_refresh(struct Data *data)
{

  if (data->refresh_worker_status < THREAD_CANCELLED && data->refresh_worker_status > THREAD_NULL) {
    data->refresh_worker_status = THREAD_CANCELLED;
    g_thread_join(refresh_worker_thread);
    return 1;
  }

  return 0;

}
