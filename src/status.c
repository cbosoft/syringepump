#define _POSIX_C_SOURCE 200809L
#include <string.h>

#include "data.h"
#include "cgl.h"
#include "status.h"
#include "util.h"


static cgl_Figure *fig = NULL;
static volatile int figure_owned = 0;
static GtkWidget *status_plot = NULL;

extern double CGL_COLOURS[][3];


void status_update(const char *oline, int linen, int draw_every)
{
  char *line = strdup(oline);

  char *tok = strtok(line, ",");
  if (!tok) goto free_and_return;
  double time = atof(tok) * 0.001;

  tok = strtok(NULL, ",");
  if (!tok) goto free_and_return;
  double force = atof(tok);

  tok = strtok(NULL, ",");
  if (!tok) goto free_and_return;
  double flowrate = atof(tok);

  while (figure_owned) ptble_usleep(100);
  figure_owned = 1;

  if (fig->lines) {
    cgl_Line *force_line = fig->lines[0];
    cgl_Line *flowrate_line = fig->lines[1];
    cgl_line_add_point(force_line, time, force);
    cgl_line_add_point(flowrate_line, time, flowrate);
    cgl_figure_scale_axes(fig);

    if (linen % draw_every == 2) {
      gtk_widget_queue_draw(status_plot);
    }
  }

  figure_owned = 0;

free_and_return:
  free(line);
}


void status_init(GObject *drw_status_plot)
{
  fig = cgl_init_figure();
  status_plot = GTK_WIDGET(drw_status_plot);
  g_signal_connect(G_OBJECT(status_plot), "draw", G_CALLBACK(cgl_painter_cb), fig);
}

void status_clear()
{
  while (figure_owned) ptble_usleep(100);

  figure_owned = 1;
  cgl_figure_clear(fig);
  for (int i = 0; i < 2 ; i++) {
    cgl_Line *line = cgl_create_line();
    cgl_line_set_colour(line, CGL_COLOURS[i%10]);
    cgl_figure_add_line(fig, line);
  }
  cgl_axes_set_xlabel(fig->axes, "time, t [s]");
  cgl_axes_set_ylabel(fig->axes, "(blue) force, F [N]\n(orange) flowrate, Q [ml/s]");
  figure_owned = 0;
}

void status_free()
{
  if (fig->nlines) {
    for (unsigned int i = 0; i < fig->nlines; i++) {
      free(fig->lines[i]);
    }
    free(fig->lines);
  }

  cgl_axes_free(fig->axes);
  free(fig);
}
