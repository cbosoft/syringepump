#include "log.h"
#include "ardiop.h"

extern int LOG_STOPPED;

void *log_update(void *vptr)
{ 
  struct thread_data *td = (struct thread_data *)vptr;
  // TODO open log file
  
  int i = 0;
  while (!LOG_STOPPED) {
    char received_text[512] = {0};
    ard_readserial_line(td->serial_fd, received_text, 512, 1000);
    
    if (i % 10 == 0) {
      // write to label
      char *current_text = (char *)gtk_label_get_text(GTK_LABEL(td->log_lbl));
      char *new_text = calloc(strlen(current_text) + strlen(received_text) + 1, sizeof(char));
      strcat(new_text, current_text);
      strcat(new_text, received_text);
      gtk_label_set_text(GTK_LABEL(td->log_lbl), new_text);
      free(new_text);
      i = 0;
    }
    i ++;

    // write to log file
    // TODO
  }

  // post log;

  return NULL;
}
