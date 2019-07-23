#include "log.h"
#include "ardiop.h"
#include "error.h"
#include "data.h"

extern int LOG_STOPPED;

void *log_update(void *void_data)
{ 
  struct Data *data = (struct Data *)void_data;

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

void append_text_to_log(struct Data *data, const char *added_text)
{
  char *current_text = (char *)gtk_label_get_text(GTK_LABEL(data->log_lbl));
  char *new_text = calloc(strlen(current_text) + 1 + strlen(added_text) + 1, sizeof(char));

  strcat(new_text, current_text);
  if (strlen(current_text))
    strcat(new_text, "\n");
  strcat(new_text, added_text);
  gtk_label_set_markup(GTK_LABEL(data->log_lbl), new_text);
  free(new_text);

  GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(data->scroll));
  gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}
