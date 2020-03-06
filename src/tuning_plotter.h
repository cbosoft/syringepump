#pragma once

#include <gtk/gtk.h>
#include "cgl.h"
#include "data.h"


void cb_file_set(GtkFileChooserButton *widget, struct Data *data);
int read_tuning_data(char *path, struct CompositionTuning **cdata);


// vim: ft=c
