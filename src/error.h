#pragma once

#define MESGLEN 256

#include "data.h"

#define IS_EMPTY_ENTRY(INP) (strlen(gtk_entry_get_text(GTK_ENTRY(INP))) == 0)
#define IS_FLOAT(INP) (strtof(gtk_entry_get_text(GTK_ENTRY(INP)), NULL) != 0)


void timestamp_error(struct Data *data, const char *fmt, ...);
void timestamp(struct Data *data, const char *fmt, ...);
int check_form(struct Data *data);

// vim: ft=c
