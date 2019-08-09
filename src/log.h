#pragma once

#include "data.h"

void append_text_to_log(struct Data *data, const char *added_text);
char *get_new_log_name(struct Data *data, int *control_type_override);
void start_log(struct Data *data);
int cancel_log(struct Data *data);

// vim: ft=c
