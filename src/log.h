#pragma once

#include "data.h"

void append_text_to_log(struct Data *data, const char *added_text);
char *get_new_log_name(struct Data *data);
void start_log(struct Data *data);
void cancel_log(struct Data *data);

// vim: ft=c
