#pragma once
#include <gtk/gtk.h>

typedef double cgl_float;
typedef unsigned int cgl_uint;

typedef struct {
  cgl_float x;
  cgl_float y;
} cgl_Point;

typedef struct {
  cgl_float r;
  cgl_float g;
  cgl_float b;
  cgl_float a;
  cgl_float w;
} cgl_LineStyle;

typedef struct {
  cgl_Point **points;
  cgl_uint npoints;
  cgl_LineStyle *style;
  char *label;
} cgl_Line;

typedef struct {
  char *x_label;
  char *y_label;
  cgl_float x_lim[2];
  cgl_float y_lim[2];
} cgl_Axes;

typedef struct {
  cgl_Axes *axes;
  cgl_Line **lines;
  cgl_uint nlines;
  /*char *title*/
} cgl_Figure;

cgl_Figure *cgl_init_figure();
void cgl_figure_scale_axes(cgl_Figure *fig);
void cgl_figure_add_line(cgl_Figure *fig, cgl_Line *line);
void cgl_figure_free(cgl_Figure *fig);
void cgl_figure_clear(cgl_Figure *fig);
void cgl_figure_plot_vector(cgl_Figure *figure, cgl_float *x, cgl_float *y, cgl_uint npoints, char *label);

cgl_Axes *cgl_create_axes();
cgl_Line *cgl_create_line();
cgl_LineStyle *cgl_create_style();
cgl_Point *cgl_create_point(cgl_float x, cgl_float y);

void cgl_style_free(cgl_LineStyle *style);

void cgl_line_add_point(cgl_Line *line, cgl_float x, cgl_float y);
void cgl_line_set_colour(cgl_Line *line, cgl_float colour[]);

char *cgl_axes_get_ylabel(cgl_Axes *ax);
char *cgl_axes_get_xlabel(cgl_Axes *ax);
void cgl_axes_set_ylabel(cgl_Axes *ax, char *lbl);
void cgl_axes_set_xlabel(cgl_Axes *ax, char *lbl);
void cgl_axes_free(cgl_Axes *ax);

void cgl_axes_get_xlim(cgl_Axes *ax, cgl_float *left, cgl_float *right);
void cgl_axes_get_ylim(cgl_Axes *ax, cgl_float *bottom, cgl_float *top);
void cgl_axes_set_xlim(cgl_Axes *ax, cgl_float left, cgl_float right);
void cgl_axes_set_ylim(cgl_Axes *ax, cgl_float bottom, cgl_float top);

gboolean cgl_painter_cb(GtkWidget *widget, cairo_t *cr, gpointer data);

// vim: ft=c
