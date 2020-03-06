#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <gtk/gtk.h>

#include "cgl.h"


cgl_float CGL_COLOURS[][3] = 
{
  {0.12156862745098039, 0.4666666666666667, 0.7058823529411765},
  {1.0, 0.4980392156862745, 0.054901960784313725},
  {0.17254901960784313, 0.6274509803921569, 0.17254901960784313},
  {0.8392156862745098, 0.15294117647058825, 0.1568627450980392},
  {0.5803921568627451, 0.403921568627451, 0.7411764705882353},
  {0.5490196078431373, 0.33725490196078434, 0.29411764705882354},
  {0.8901960784313725, 0.4666666666666667, 0.7607843137254902},
  {0.4980392156862745, 0.4980392156862745, 0.4980392156862745},
  {0.7372549019607844, 0.7411764705882353, 0.13333333333333333},
  {0.09019607843137255, 0.7450980392156863, 0.8117647058823529}
};
cgl_uint CGL_N_COLOURS = 10;


cgl_Figure *cgl_init_figure()
{
  cgl_Figure *fig = calloc(1, sizeof(cgl_Figure));
  fig->axes = cgl_create_axes();
  return fig;
}
void cgl_axes_get_xlim(cgl_Axes *ax, cgl_float *left, cgl_float *right)
{
  (*left) = ax->x_lim[0];
  (*right) = ax->x_lim[1];
}

void cgl_axes_get_ylim(cgl_Axes *ax, cgl_float *bottom, cgl_float *top)
{
  (*bottom) = ax->y_lim[0];
  (*top) = ax->y_lim[1];
}

void cgl_axes_set_xlim(cgl_Axes *ax, cgl_float left, cgl_float right)
{
  ax->x_lim[0] = left;
  ax->x_lim[1] = right;
}

void cgl_axes_set_ylim(cgl_Axes *ax, cgl_float bottom, cgl_float top)
{
  ax->y_lim[0] = bottom;
  ax->y_lim[1] = top;
}

void cgl_figure_scale_axes(cgl_Figure *fig)
{
  cgl_float x_min = DBL_MAX, x_max = DBL_MIN, y_min = DBL_MAX, y_max = DBL_MIN;
  for (cgl_uint i = 0; i < fig->nlines; i++) {
    cgl_Line *line = fig->lines[i];

    if (line->npoints < 2) {

      if (1.0 > x_max)
        x_max = 1.0;
      if (0.0 < x_min)
        x_min = 0.0;

      if (1.0 > y_max)
        y_max = 1.0;
      if (0.0 < y_min)
        y_min = 0.0;

      continue;
    }

    for (cgl_uint j = 0; j < line->npoints; j++) {
      cgl_Point *point = line->points[j];

      if (point->x > x_max)
        x_max = point->x;
      if (point->x < x_min)
        x_min = point->x;

      if (point->y > y_max)
        y_max = point->y;
      if (point->y < y_min)
        y_min = point->y;
    }
  }

  cgl_axes_set_xlim(fig->axes, x_min, x_max);
  cgl_axes_set_ylim(fig->axes, y_min, y_max);
}

void cgl_figure_add_line(cgl_Figure *fig, cgl_Line *line)
{
  fig->lines = realloc(fig->lines, (++fig->nlines)*sizeof(cgl_Line));
  fig->lines[fig->nlines-1] = line;
}

void cgl_point_free(cgl_Point *point)
{
  free(point);
}

void cgl_line_free(cgl_Line *line)
{
  for (cgl_uint i = 0; i < line->npoints; i++) {
    cgl_point_free(line->points[i]);
  }
  free(line->points);

  if (line->label)
    free(line->label);

  if (line->style)
    cgl_style_free(line->style);

  free(line);
}

void cgl_figure_clear(cgl_Figure *fig)
{
  if (fig->lines) {
    for (cgl_uint i = 0; i < fig->nlines; i++) {
      cgl_line_free(fig->lines[i]);
    }
    fig->lines = NULL;
    fig->nlines = 0;
  }

  cgl_axes_free(fig->axes);
  fig->axes = cgl_create_axes();
}


// TODO multiple x and or y axes
// nY axes, so that multiply plots can be shown on the same graph accurately
cgl_Axes *cgl_create_axes()
{
  cgl_Axes *ax = calloc(1, sizeof(cgl_Axes));
  ax->x_label = calloc(2, sizeof(char));
  ax->x_label[0] = 'x';
  ax->y_label = calloc(2, sizeof(char));
  ax->y_label[0] = 'y';
  ax->x_lim[0] = 0.0;
  ax->x_lim[1] = 1.0;
  ax->y_lim[0] = 0.0;
  ax->y_lim[1] = 1.0;
  return ax;
}

void cgl_axes_free(cgl_Axes *ax)
{
  free(ax->x_label);
  free(ax->y_label);
  free(ax);
}

cgl_float cgl_axes_get_span_x(cgl_Axes *ax)
{
  cgl_float max = ax->x_lim[1];
  cgl_float min = ax->x_lim[0];
  return max - min;
}

cgl_float cgl_axes_get_span_y(cgl_Axes *ax)
{
  cgl_float max = ax->y_lim[1];
  cgl_float min = ax->y_lim[0];
  return max - min;
}


cgl_LineStyle *cgl_create_style()
{
  cgl_LineStyle *style = calloc(1, sizeof(cgl_LineStyle));
  style->r = 0.0;
  style->g = 0.0;
  style->b = 1.0;
  style->a = 1.0;
  style->w = 2.0;
  return style;
}

void cgl_style_free(cgl_LineStyle *style)
{
  free(style);
}

cgl_Line *cgl_create_line()
{
  cgl_Line *line = calloc(1, sizeof(cgl_Line));
  line->style = cgl_create_style();
  return line;
}



void line_to(cairo_t *cr, guint x1, guint y1, guint x2, guint y2)
{
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);
}


void cgl_figure_plot_vector(cgl_Figure *figure, cgl_float *x, cgl_float *y, cgl_uint npoints, char *label)
{
  cgl_Line *line = cgl_create_line();
  line->points = calloc(npoints, sizeof(cgl_Point*));
  // TODO error checking on alloc
  line->label = strdup(label);
  line->npoints = npoints;
  
  for (cgl_uint point_i = 0; point_i < npoints; point_i++) {
    line->points[point_i] = cgl_create_point(x[point_i], y[point_i]);
  }

  figure->lines = realloc(figure->lines, (++figure->nlines)*sizeof(cgl_Line));
  cgl_uint colour_index = (figure->nlines-1) % CGL_N_COLOURS;
  cgl_line_set_colour(line, CGL_COLOURS[colour_index]);
  figure->lines[figure->nlines-1] = line;
}


cgl_Point *cgl_create_point(cgl_float x, cgl_float y)
{
  cgl_Point *point = calloc(1, sizeof(cgl_Point));
  point->x = x;
  point->y = y;
  return point;
}

void cgl_line_add_point(cgl_Line *line, cgl_float x, cgl_float y)
{
  line->points = realloc(line->points, (++line->npoints)*sizeof(cgl_Point));
  line->points[line->npoints-1] = cgl_create_point(x, y);
}

void cgl_line_set_colour(cgl_Line *line, cgl_float colour[])
{
  line->style->r = colour[0];
  line->style->g = colour[1];
  line->style->b = colour[2];
}

gboolean cgl_painter_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  (void) widget;
  cgl_Figure *fig = (cgl_Figure *)data;

  if (fig == NULL)
    return FALSE;

  guint width = gtk_widget_get_allocated_width(widget);
  guint height = gtk_widget_get_allocated_height(widget);

  //cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 12);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, "words", &extents);
  cgl_uint text_height = extents.height;

  // Background
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);

  cgl_uint margin_left = (guint)(0.15*width);
  cgl_uint margin_right = (guint)(0.1*width);
  cgl_uint margin_top = (guint)(0.1*height);
  cgl_uint margin_bottom = (guint)(0.25*height);

  cgl_float px_per_data_x = ((cgl_float)(width-margin_left-margin_right)) / cgl_axes_get_span_x(fig->axes);
  cgl_float px_per_data_y = ((cgl_float)(height-margin_top-margin_bottom)) / cgl_axes_get_span_y(fig->axes);

  // Axes
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cgl_uint axes_x_length = width - margin_left - margin_right;
  cgl_uint axes_y_length = height - margin_top - margin_bottom;
  line_to(cr, margin_left-2, height-margin_bottom+2, width-margin_right+2, height-margin_bottom+2);
  line_to(cr, margin_left-2, height-margin_bottom+2, margin_left-2, margin_top-2);
  cairo_stroke(cr);

  // Ticks
  //cairo_set_line_width(cr, 1.0);
  cgl_uint n_x_ticks = 3, n_y_ticks = 4;
  cgl_uint x_tick_dp = axes_x_length/(n_x_ticks-1);
  cgl_uint y_tick_dp = axes_y_length/(n_y_ticks-1);
  for (cgl_uint i = 0; i < n_x_ticks; i++) {
    cairo_move_to(cr, margin_left+(i*x_tick_dp), height-margin_bottom+2);
    cairo_line_to(cr, margin_left+(i*x_tick_dp), height-margin_bottom+7);
    cairo_stroke(cr);

    char ticklabel[100] = {0};
    snprintf(ticklabel, 99, "%.02f", (i*x_tick_dp)/px_per_data_x);
    cairo_text_extents(cr, ticklabel, &extents);
    cairo_move_to(cr, margin_left+(i*x_tick_dp)-extents.width/2, height-margin_bottom+10+extents.height);
    cairo_show_text(cr, ticklabel);
    cairo_stroke(cr);
  }
  for (cgl_uint i = 0; i < n_y_ticks; i++) {
    cairo_move_to(cr, margin_left-2, margin_top+(i*y_tick_dp));
    cairo_line_to(cr, margin_left-7, margin_top+(i*y_tick_dp));
    cairo_stroke(cr);

    char ticklabel[100] = {0};
    snprintf(ticklabel, 99, "%.02f", (i*y_tick_dp)/px_per_data_y);
    cairo_text_extents(cr, ticklabel, &extents);
    cairo_move_to(cr, margin_left-extents.width-10, height-margin_bottom+2-(i*y_tick_dp));
    cairo_show_text(cr, ticklabel);
    cairo_stroke(cr);
  }

  // Labels
  cairo_text_extents(cr, fig->axes->x_label, &extents);
  cairo_move_to(cr, margin_left + axes_x_length/2 - extents.width/2, height-(margin_bottom/2));
  cairo_show_text(cr, fig->axes->x_label);

  cairo_save(cr); 
  cairo_rotate(cr, -0.5*3.14);
  cairo_text_extents(cr, fig->axes->y_label, &extents);
  //cairo_move_to(cr, (margin_left-extents.width)/2, margin_top + axes_y_length/2); // without rotation
  cairo_move_to(cr, margin_top + axes_y_length/2, margin_right + axes_x_length + margin_left/2);
  //cairo_move_to(cr, width/2 - extents.width/2, height-(margin_bottom/2));
  cairo_show_text(cr, fig->axes->y_label);
  cairo_restore(cr);


  cgl_uint legend = FALSE, legend_box_height = 0, legend_box_width = 0;
  // Data
#define CGL_SCALE_X(F) (cgl_uint)(px_per_data_x * (F))
#define CGL_SCALE_Y(F) (cgl_uint)(px_per_data_y * (F))
  for (cgl_uint line_i = 0; line_i < fig->nlines; line_i++) {
    cgl_Line *line = fig->lines[line_i];
    cairo_set_line_width(cr, line->style->w);
    if (!line->npoints)
      continue;

    cgl_Point *point = line->points[0];
    cairo_move_to(cr, margin_left + CGL_SCALE_X(point->x - fig->axes->x_lim[0]), height-(margin_bottom+CGL_SCALE_Y(point->y - fig->axes->y_lim[0])));
    // TODO draw point if style requires it

    cairo_set_source_rgba(cr, line->style->r, line->style->g, line->style->b, line->style->a);

    for (cgl_uint point_i = 1; point_i < line->npoints; point_i++) {
      point = line->points[point_i];
      cairo_line_to(cr, margin_left + CGL_SCALE_X(point->x - fig->axes->x_lim[0]), height-(margin_bottom+CGL_SCALE_Y(point->y - fig->axes->y_lim[0])));
    }
    cairo_stroke(cr);


    if (line->label) {
      legend = TRUE;
      cairo_text_extents(cr, line->label, &extents);
      if (extents.width > legend_box_width) {
        legend_box_width = extents.width;
      }
    }
  }
#undef CGL_SCALE_Y
#undef CGL_SCALE_X

  // TODO legend
  if (legend) {
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    legend_box_height = (fig->nlines+1) * (text_height + 5);
    legend_box_width = 30 + legend_box_width;
    cairo_move_to(cr, width - 5 - legend_box_width, height/2 - legend_box_height/2);
    cairo_line_to(cr, width - 5 - legend_box_width, height/2 + legend_box_height/2);
    cairo_line_to(cr, width - 5,                    height/2 + legend_box_height/2);
    cairo_line_to(cr, width - 5,                    height/2 - legend_box_height/2);
    cairo_line_to(cr, width - 5 - legend_box_width, height/2 - legend_box_height/2);
    cairo_stroke(cr);

    for (cgl_uint i = 0; i < fig->nlines; i++) {
      cgl_Line *line = fig->lines[i];
      if (!line->label)
        continue;

      cairo_set_line_width(cr, line->style->w);

      // paint small section of line
      cairo_set_source_rgba(cr, line->style->r, line->style->g, line->style->b, line->style->a);
      cairo_move_to(cr, width - 0 - legend_box_width, height/2 - legend_box_height/2 + (i+1)*(text_height+5));
      cairo_line_to(cr, width + 5 - legend_box_width, height/2 - legend_box_height/2 + (i+1)*(text_height+5));
      cairo_stroke(cr);

      cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
      cairo_move_to(cr, width + 10 - legend_box_width, height/2 - legend_box_height/2 + (i+1)*(text_height+5) + 2.5);
      cairo_show_text(cr, line->label);
    }
  }
  return FALSE;
}



char *cgl_axes_get_ylabel(cgl_Axes *ax)
{
  return ax->y_label;
}


char *cgl_axes_get_xlabel(cgl_Axes *ax)
{
  return ax->y_label;
}


void cgl_axes_set_ylabel(cgl_Axes *ax, char *lbl)
{
  free(ax->y_label);
  ax->y_label = strdup(lbl);
}


void cgl_axes_set_xlabel(cgl_Axes *ax, char *lbl)
{
  free(ax->x_label);
  ax->x_label = strdup(lbl);
}


void cgl_figure_free(cgl_Figure *fig)
{
  if (fig->lines) {
    for (unsigned int i = 0; i < fig->nlines; i++) {
      cgl_line_free(fig->lines[i]);
    }
    free(fig->lines);
  }
  cgl_axes_free(fig->axes);
  free(fig);
}
