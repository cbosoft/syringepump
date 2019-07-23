#include "log.h"
#include "ardiop.h"
#include "error.h"
#include "data.h"

extern int LOG_STOPPED;
static char *log_string = NULL;




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
