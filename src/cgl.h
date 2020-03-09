#pragma once
#include <gtk/gtk.h>

typedef struct {
  double x;
  double y;
} cgl_Point;

typedef struct {
  double r;
  double g;
  double b;
  double a;
  double w;
} cgl_LineStyle;

typedef struct {
  cgl_Point **points;
  unsigned int npoints;
  cgl_LineStyle *style;
  char *label;
  unsigned int y_ax_index;
} cgl_Line;

typedef struct {
  char *x_label;
  char *y_label;
  char *alt_y_label;
  double x_lim[2];
  double y_lim[2];
  double alt_y_lim[2];
} cgl_Axes;

typedef struct {
  cgl_Axes *axes;
  cgl_Line **lines;
  unsigned int nlines;
  /*char *title*/
} cgl_Figure;

cgl_Figure *cgl_init_figure();
void cgl_figure_scale_axes(cgl_Figure *fig);
void cgl_figure_add_line(cgl_Figure *fig, cgl_Line *line);
void cgl_figure_free(cgl_Figure *fig);
void cgl_figure_clear(cgl_Figure *fig);
void cgl_figure_plot_vector(cgl_Figure *figure, double *x, double *y, unsigned int npoints, char *label);

cgl_Axes *cgl_create_axes();
cgl_Line *cgl_create_line();
cgl_LineStyle *cgl_create_style();
cgl_Point *cgl_create_point(double x, double y);

void cgl_style_free(cgl_LineStyle *style);

void cgl_line_add_point(cgl_Line *line, double x, double y);
void cgl_line_set_colour(cgl_Line *line, double colour[]);

char *cgl_axes_get_ylabel(cgl_Axes *ax);
char *cgl_axes_get_xlabel(cgl_Axes *ax);
void cgl_axes_set_ylabel(cgl_Axes *ax, char *lbl);
void cgl_axes_set_alt_ylabel(cgl_Axes *ax, char *lbl);
void cgl_axes_set_ylabel(cgl_Axes *ax, char *lbl);
void cgl_axes_set_xlabel(cgl_Axes *ax, char *lbl);
void cgl_axes_free(cgl_Axes *ax);

void cgl_axes_get_xlim(cgl_Axes *ax, double *left, double *right);
void cgl_axes_get_ylim(cgl_Axes *ax, double *bottom, double *top);
void cgl_axes_set_xlim(cgl_Axes *ax, double left, double right);
void cgl_axes_set_ylim(cgl_Axes *ax, double bottom, double top);

int cgl_figure_does_use_alt_y(cgl_Figure *fig);

gboolean cgl_painter_cb(GtkWidget *widget, cairo_t *cr, gpointer data);

// vim: ft=c
