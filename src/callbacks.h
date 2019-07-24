#pragma once
#include <gtk/gtk.h>
#include "data.h"

void cb_lbl_size_changed(GObject *obj, GdkRectangle *allocation, struct Data *data);
void cb_connect(GObject *obj, struct Data *data);
void cb_disconnect(GObject *obj, struct Data *data);
void cb_quit(GObject *obj, struct Data *data);
void cb_refresh_serial(GObject *obj, struct Data *data);

// vim: ft=c
