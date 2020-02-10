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
  long nchars = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = calloc(nchars+1, sizeof(char));
  if (!buffer)
    return 1;
  fread(buffer, 1, nchars, f);
  fclose(f);

  long nlines = 0;
  for (int i = 0; i < nchars; i++)
    if (buffer[i] == '\n') nlines ++;

  (*cdata) = malloc(sizeof(struct CompositionTuning));
  (*cdata)->n = nlines;
  (*cdata)->cm = malloc(nlines*sizeof(cgl_float));
  (*cdata)->kp = malloc(nlines*sizeof(cgl_float));
  (*cdata)->ki = malloc(nlines*sizeof(cgl_float));
  (*cdata)->kd = malloc(nlines*sizeof(cgl_float));


  char **lines = malloc(nlines*sizeof(char*));
  char *line_buffer = strtok(buffer, "\n");
  for (int i = 0; i < nlines; i++) {
    lines[i] = strdup(line_buffer);
    line_buffer = strtok(NULL, "\n");
  }
  free(buffer);

  for (int i = 0; i < nlines; i++) {
    char *line = lines[i];
    fprintf(stderr, "%s\n", line);
    (*cdata)->cm[i] = atof(strtok(line, ","));
    (*cdata)->kp[i] = atof(strtok(NULL, ","));
    (*cdata)->ki[i] = atof(strtok(NULL, ","));
    (*cdata)->kd[i] = atof(strtok(NULL, ","));
    fprintf(stderr, "%f\n", (*cdata)->kd[i]);
    free(line);
  }
  free(lines);

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
