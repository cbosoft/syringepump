#pragma once

#include <gtk/gtk.h>
#include "cgl.h"
#include "data.h"


void cb_file_set(GtkFileChooserButton *widget, struct Data *data);
int read_tuning_data(char *path, struct CompositionTuning **cdata);
void composition_data_free(struct CompositionTuning *cata);
void init_tuning_plot(struct Data *data);
void free_tuning_plot();


// vim: ft=c
