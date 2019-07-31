#pragma once

#define MESGLEN 256

#include "data.h"

void timestamp_error(struct Data *data, int is_gui, const char *fmt, ...);
void timestamp(struct Data *data, int is_gui, const char *fmt, ...);

// vim: ft=c
