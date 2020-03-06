#pragma once

#include "data.h"

void status_update(const char *oline, int nline, int drawevery);
void status_init(GObject *drw_status_plot);
void status_clear();
void status_free();

// vim: ft=c
