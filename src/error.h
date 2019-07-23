#pragma once

#define MESGLEN 256

#include "data.h"



void timestamp_error(struct Data *data, const char *fmt, ...);
void timestamp(struct Data *data, const char *fmt, ...);

// vim: ft=c
