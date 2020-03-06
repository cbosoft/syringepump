#pragma once
#include <gtk/gtk.h>
#include "data.h"

void cb_lbl_size_changed(GObject *obj, GdkRectangle *allocation, struct Data *data);
void cb_begin_clicked(GObject *obj, struct Data *data);
void cb_stop_clicked(GObject *obj, struct Data *data);
void cb_quit_clicked(GObject *obj, struct Data *data);
void cb_refresh_clicked(GObject *obj, struct Data *data);
void cb_tag_text_changed(GObject *obj, struct Data *data);
void cb_tab_page_changed(GObject *obj, GtkWidget *page, guint page_num, struct Data *data);
void cb_setter_radio_changed(GObject *obj, struct Data *data);
void cb_tuning_clicked(GObject *obj, struct Data *data);
void cb_pid_manual_radio_changed(GObject *obj, struct Data *data);

// vim: ft=c
