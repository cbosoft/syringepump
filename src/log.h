#pragma once
#include <gtk/gtk.h>

#include "data.h"

void append_text_to_log(struct Data *data, const char *added_text);
char *get_new_log_name(struct Data *data);

// vim: ft=c
