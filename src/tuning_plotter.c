#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include "cgl.h"


#include "tuning_plotter.h"
#include "cJSON.h"
#include "data.h"
#include "form.h"
#include "error.h"

static cgl_Figure *fig = NULL;

void cb_tuning_clicked(GObject *obj, struct Data *data)
{
  (void) obj;

  int control_type = form_get_control_type(data), response = 1;
  char *kp = NULL;
  char *ki = NULL;
  char *kd = NULL;

  fig = cgl_init_figure();
  g_signal_connect(G_OBJECT(get_object_safe(data, "drawPlotter")), 
      "draw", G_CALLBACK(cgl_painter_cb), fig);
  struct CompositionTuning *cdata = data->composition_data;
  cgl_figure_plot_vector(fig, cdata->cm, cdata->kp, cdata->n, "Kp");
  cgl_figure_plot_vector(fig, cdata->cm, cdata->ki, cdata->n, "Ki");
  cgl_figure_plot_vector(fig, cdata->cm, cdata->kd, cdata->n, "Kd");
  cgl_axes_set_ylabel(fig->axes, "Value");
  cgl_axes_set_xlabel(fig->axes, "Composition (frac CS in solvent)");
  cgl_figure_scale_axes(fig);

  switch (control_type) {
    case FORM_CONTROL_NONE:
      timestamp_error(data, 0, "this should never happen.");
      return;

    case FORM_CONTROL_MEAS:
    case FORM_CONTROL_PID:
      kp = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKP"))));
      ki = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKI"))));
      kd = strdup(gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entKD"))));
      response = gtk_dialog_run(GTK_DIALOG(get_object_safe(data, "winTuningDialog")));
      if (response != GTK_RESPONSE_ACCEPT) {
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKP")), kp);
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKI")), ki);
          gtk_entry_set_text(GTK_ENTRY(get_object_safe(data, "entKD")), kd);
      }
      gtk_widget_hide(GTK_WIDGET(get_object_safe(data, "winTuningDialog")));
      free(kp);
      free(ki);
      free(kd);
      break;

      cgl_figure_free(fig);
  }

}

int read_tuning_data(char *path, struct CompositionTuning **cdata)
{
  FILE *f = fopen (path, "rb");
  if (!f)
    return 1;

  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = calloc(length+1, sizeof(char));
  if (!buffer)
    return 1;
  fread(buffer, 1, length, f);

  fclose(f);

  cJSON *tuning_data = cJSON_Parse(buffer);
  cJSON *composition_array = cJSON_GetObjectItem(tuning_data, "compositions");

  if (*cdata) {
    // TODO free the rest
    free(*cdata);
  }
  (*cdata) = malloc(sizeof(struct CompositionTuning));
  (*cdata)->n = cJSON_GetArraySize(composition_array);
  (*cdata)->cm = malloc((*cdata)->n*sizeof(cgl_float));
  (*cdata)->kp = malloc((*cdata)->n*sizeof(cgl_float));
  (*cdata)->ki = malloc((*cdata)->n*sizeof(cgl_float));
  (*cdata)->kd = malloc((*cdata)->n*sizeof(cgl_float));
  cJSON *composition_data = NULL;
  cgl_uint i = 0;
  cJSON_ArrayForEach(composition_data, composition_array) {
    cJSON *cm_json = cJSON_GetObjectItem(composition_data, "cm");
    cJSON *kp_json = cJSON_GetObjectItem(composition_data, "kp");
    cJSON *ki_json = cJSON_GetObjectItem(composition_data, "ki");
    cJSON *kd_json = cJSON_GetObjectItem(composition_data, "kd");
    (*cdata)->cm[i] = cm_json->valuedouble;
    (*cdata)->kp[i] = kp_json->valuedouble;
    (*cdata)->ki[i] = ki_json->valuedouble;
    (*cdata)->kd[i] = kd_json->valuedouble;
    i++;
  }

  free(buffer);

  return 0;
}


void cb_file_set(GtkFileChooserButton *widget, struct Data *data)
{
  (void) data;
  char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

  if (fig->nlines)
    free(fig->lines);
  fig->nlines = 0;

  if (read_tuning_data(path, &data->composition_data))
    return;

  struct CompositionTuning *cdata = data->composition_data;
  cgl_figure_clear(fig);
  cgl_figure_plot_vector(fig, cdata->cm, cdata->kp, cdata->n, "Kp");
  cgl_figure_plot_vector(fig, cdata->cm, cdata->ki, cdata->n, "Ki");
  cgl_figure_plot_vector(fig, cdata->cm, cdata->kd, cdata->n, "Kd");
  cgl_figure_scale_axes(fig);
}




double interp(double *xvec, double *yvec, unsigned int n, double xat)
{
  for (unsigned int i = 1; i < n; i++) {
    if (xat < xvec[i]) {
      return xat*(yvec[i] - yvec[i-1]) / (xvec[i] - xvec[i-1]);
    }
  }
  return -1.0;
}
