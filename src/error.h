#pragma once

#define MESGLEN 256
#define ERROR_BUFFER_SIZE 1000

#include "data.h"


void timestamp_error(struct Data *data, int is_gui, int show_errno, const char *fmt, ...);
void timestamp(struct Data *data, int is_gui, const char *fmt, ...);

// vim: ft=c
