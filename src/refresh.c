#include <gtk/gtk.h>
#include <glob.h>
#include <stdlib.h>

#include "refresh.h"
#include "threads.h"
#include "error.h"
#include "form.h"
#include "errno.h"
#include "util.h"



static GThread *refresh_worker_thread = NULL;
static int number_failed_attempts = 0;

// struct sensitive_callback_data {
//   struct Data *data;
//   int sensitive;
// };
// 
// static gboolean sensitive_callback(struct sensitive_callback_data *acd)
// {
//   GObject *serial_combo_box = get_object_safe(acd->data, "cmbSerial");
//   gtk_widget_set_sensitive(GTK_WIDGET(serial_combo_box), acd->sensitive);
// 
//   return 0;
// }


static gboolean cmb_clear_callback(GObject *cmb)
{
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(cmb));

  return 0;
}

struct cmb_fill_callback_data{
  GObject *cmb;
  glob_t *glob_ptr;
};

static gboolean cmb_fill_callback(struct cmb_fill_callback_data *cfd)
{
  for (size_t i = 0; i < cfd->glob_ptr->gl_pathc; i++) {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cfd->cmb),
        cfd->glob_ptr->gl_pathv[i]);
    fprintf(stderr, "%s\n", cfd->glob_ptr->gl_pathv[i]);
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(cfd->cmb), 0);

  globfree(cfd->glob_ptr);
  free(cfd->glob_ptr);
  free(cfd);

  return 0;
}



static void *refresh_worker(void *vptr_data)
{
  struct Data *data = (struct Data *)vptr_data;

  data->refresh_worker_status = THREAD_STARTED;

  form_set_sensitive(data, FORM_REFRESHING);

  timestamp(data, 0, "Searching for Arduino...");

  ptble_usleep(300);
  g_idle_add((GSourceFunc)cmb_clear_callback, get_object_safe(data, "cmbSerial"));


  const char *dev = "/dev/ttyACM*";
  glob_t *glob_res = calloc(1, sizeof(glob_t));
  glob(dev, 0, NULL, &(*glob_res));
  errno = 0; // clear accumulated access errors

  if (glob_res->gl_pathc == 0) {
    number_failed_attempts ++;

    if (number_failed_attempts > 3) {
      timestamp_error(data, 0, 1, "No Arduino found! If its still not showing up, try rebooting.");
    }
    else {
      timestamp_error(data, 0, 1, "No Arduino found! Is it definitely plugged in?");
    }

    globfree(glob_res);
    free(glob_res);
    form_set_sensitive(data, FORM_NOSERIAL);
    data->refresh_worker_status = THREAD_STOPPED;
    return NULL;
  }
  else {
    struct cmb_fill_callback_data *cfd = calloc(1, sizeof(struct cmb_fill_callback_data));
    cfd->cmb = get_object_safe(data, "cmbSerial");
    cfd->glob_ptr = glob_res;
    g_idle_add((GSourceFunc)cmb_fill_callback, cfd);


    if (glob_res->gl_pathc == 1) {
      timestamp(data, 0, "Arduino found!");
    }
    else {
      timestamp(data, 0, "Multiple possible Arduino found.");
    }
  }


  form_set_sensitive(data, FORM_DISCONNECTED);
  data->refresh_worker_status = THREAD_STOPPED;

  // {
  //   struct sensitive_callback_data *scd = calloc(1, sizeof(struct sensitive_callback_data));
  //   scd->data = data;
  //   scd->sensitive = 0;
  //   g_idle_add((GSourceFunc)sensitive_callback, scd);
  // }

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
