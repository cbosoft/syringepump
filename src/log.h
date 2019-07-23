#pragma once
#include <gtk/gtk.h>

#include "data.h"

void *log_update(void *vptr);
void append_text_to_log(struct Data *data, const char *added_text);

// vim: ft=c
