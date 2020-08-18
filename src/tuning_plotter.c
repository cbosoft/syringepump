#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cgl.h"
#include "tuning_plotter.h"
#include "cJSON.h"
#include "data.h"
#include "form.h"
#include "error.h"

static cgl_Figure *fig = NULL;

void init_tuning_plot(struct Data *data)
{
  fig = cgl_init_figure();
  g_signal_connect(G_OBJECT(get_object_safe(data, "drawPlotter")),
      "draw", G_CALLBACK(cgl_painter_cb), fig);

  struct CompositionTuning *cdata = data->composition_data;

  if (cdata) {
    cgl_figure_plot_vector(fig, cdata->cm, cdata->kp, cdata->n, "Kp");
    cgl_figure_plot_vector(fig, cdata->cm, cdata->ki, cdata->n, "Ki");
    cgl_figure_plot_vector(fig, cdata->cm, cdata->kd, cdata->n, "Kd");
  }

  cgl_axes_set_ylabel(fig->axes, "Value");
  cgl_axes_set_xlabel(fig->axes, "Composition (frac CS in solvent)");
  cgl_figure_scale_axes(fig);
}

void free_tuning_plot()
{
  cgl_figure_free(fig);
}

void composition_data_free(struct CompositionTuning *cdata)
{
  free(cdata->cm);
  free(cdata->kp);
  free(cdata->ki);
  free(cdata->kd);
  free(cdata);
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

  if (*cdata)
    composition_data_free(*cdata);

  (*cdata) = malloc(sizeof(struct CompositionTuning));
  (*cdata)->n = nlines;
  (*cdata)->cm = malloc(nlines*sizeof(double));
  (*cdata)->kp = malloc(nlines*sizeof(double));
  (*cdata)->ki = malloc(nlines*sizeof(double));
  (*cdata)->kd = malloc(nlines*sizeof(double));


  char **lines = malloc(nlines*sizeof(char*));
  char *line_buffer = strtok(buffer, "\n");
  for (int i = 0; i < nlines; i++) {
    lines[i] = strdup(line_buffer);
    line_buffer = strtok(NULL, "\n");
  }
  free(buffer);

  for (int i = 0; i < nlines; i++) {
    char *line = strdup(lines[i]);
    (*cdata)->cm[i] = atof(strtok(line, ","));
    (*cdata)->kp[i] = atof(strtok(NULL, ","));
    (*cdata)->ki[i] = atof(strtok(NULL, ","));
    (*cdata)->kd[i] = atof(strtok(NULL, ","));
    free(line);
    free(lines[i]);
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
