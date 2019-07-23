#include "log.h"
#include "ardiop.h"
#include "error.h"
#include "data.h"

extern int LOG_STOPPED;
static char *log_string = NULL;




void *log_update(void *void_data)
{ 
  struct Data *data = (struct Data *)void_data;
  
  timestamp(data, "Waiting for Arduino...");
  int arduino_ready = 0, rv = 0;
  while (!arduino_ready) {
    char received_text[512] = {0};

    if ( (rv = ard_readserial_line(data->serial_fd, received_text, 512, 1000)) ) {
      timestamp(NULL, "Error reading from serial (%d)", rv);
    }
    timestamp(NULL, received_text);

    if (strcmp(received_text, "START") == 0) {
      arduino_ready = 1;
    }

  }
  timestamp(data, "Arduino ready, starting!");

  // TODO open log file

  int i = 0;
  LOG_STOPPED = 0;
  while (!LOG_STOPPED) {
    char received_text[512] = {0};
    ard_readserial_line(data->serial_fd, received_text, 512, 1000);

    timestamp(data, "rec'd \"%s\"", received_text);

    if (i % 200 == 0) {
      append_text_to_log(data, received_text);
      i = 0;
    }

    i++;

    // write to log file
    // TODO
  }

  // post log;
  // TODO close log file

  return NULL;
}

void append_text_to_log(struct Data *data, const char *added_markup)
{

  if (log_string == NULL) {
    log_string = calloc(strlen(added_markup) + 1, sizeof(char));
    strcpy(log_string, added_markup);
  }
  else {
    log_string = realloc(log_string, strlen(log_string) + 1 + strlen(added_markup) + 1);
    strcat(log_string, "\n");
    strcat(log_string, added_markup);
  }

  gtk_label_set_markup(GTK_LABEL(data->log_lbl), log_string);

  GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(data->scroll));
  gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}
