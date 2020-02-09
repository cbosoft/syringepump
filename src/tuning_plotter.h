#pragma once

#include <gtk/gtk.h>
#include <cgl.h>
#include "data.h"


void cb_file_set(GtkFileChooserButton *widget, struct Data *data);
int read_tuning_data(char *path, struct CompositionTuning **cdata);
void cb_pid_manual_radio_changed(GObject *obj, struct Data *data);
double interp(double *xvec, double *yvec, unsigned int n, double xat);


// vim: ft=c
