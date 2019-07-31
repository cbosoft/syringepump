#include <time.h>
#include <errno.h>
#include <string.h>
#include <glob.h>

#include "log.h"
#include "error.h"
#include "data.h"
#include "threads.h"
#include "disconnect.h"
#include "serial.h"




extern int log_worker_status;
static char *log_string = NULL;
static GThread *log_worker_thread;




static void *log_worker(void *void_data)
{ 
  log_worker_status = THREAD_STARTED;
  struct Data *data = (struct Data *)void_data;
  const int print_every = 10;
  
  timestamp(data, 0, "Waiting for Arduino...");
  switch (wait_for(data, 0, "START", 100, &log_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, "Arduino connection timed out!");
      return NULL;
  }
  timestamp(data, 0, "Arduino ready, starting!");

  get_new_log_name(data);
  FILE *fp = fopen(data->logpath, "w");

  int lineno = 0, charno = 0, timeout = 1000;
  
  struct timespec ms_span;
  ms_span.tv_sec = 0;
  ms_span.tv_nsec = 1000*1000;

  while (log_worker_status < THREAD_CANCELLED) {
    char received_text[512] = {0};
    char b[1];
    charno = 0;
    do {
      int n = read(data->serial_fd, b, 1);

      if(n == -1) {
        // error
        timestamp_error(NULL, 0, 
            "something went wrong reading a byte (read failed) (%d) %s", 
            errno, strerror(errno));
        exit(1); // TODO handle properly
      }
      else if(n == 0) {
        // no read
        nanosleep(&ms_span, NULL);  // wait 1 msec try again
        timeout--;
        if(timeout == 0){
          timestamp_error(NULL, 0,  
              "something went wrong reading a byte (timed out) (%d), %s", 
              errno, strerror(errno));
          exit(1); // TODO handle properly
        }

        continue;
      }

      received_text[charno] = b[0];

      charno++;

    } while(b[0] != '\n' && charno < 512 && (log_worker_status < THREAD_CANCELLED));

    received_text[charno-1] = 0;

    if (received_text[0] == 'P') {
      timestamp(NULL, " :: %s", received_text);
    }
    else {
      timestamp( 
          ((lineno % print_every) == 0) ? data : NULL,
          "R: %s",
          received_text);
      lineno = 
        (lineno % print_every == 0) ? (1) : (lineno + 1);
    }

    if (strcmp(received_text, "STOP") == 0) {
      // arduino requests stop
      timestamp(data, 0, "Arduino finished!");
      log_worker_status = THREAD_STOPPED;
    }
    else {
      // write to log file
      fprintf(fp, "%s\n", received_text);
    }

  }

  fclose(fp);

  disconnect(data);

  return NULL;
}




void start_log(struct Data *data)
{
  log_worker_thread = g_thread_new("log_thread", log_worker, data);
}




void append_text_to_log(struct Data *data, const char *added_markup)
{

  if (log_string == NULL) {

    log_string = calloc(
        strlen(added_markup) + 1, 
        sizeof(char));

    strcpy(log_string, added_markup);

  }
  else {
    
    log_string = realloc(
        log_string, 
        strlen(log_string) + 1 + strlen(added_markup) + 1);

    strcat(log_string, "\n");
    strcat(log_string, added_markup);

  }

  gtk_label_set_markup(GTK_LABEL(data->log_lbl), log_string);

}




char *get_new_log_name(struct Data *data)
{
  char *log_dir = gtk_file_chooser_get_current_folder(
      GTK_FILE_CHOOSER(data->log_folder_fch));

  const char *pref = "syrpu";

  char *date = calloc(15, sizeof(char));
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(date, 50, "%Y-%m-%d", timeinfo);

  data->tag = (char *)gtk_entry_get_text(GTK_ENTRY(data->tag_inp));

  char *pattern = calloc(256, sizeof(char));
  sprintf(pattern, 
      "%s/%s_%s(*)_SP%f-KP%f-KI%f-KD%f_%s.csv", 
      log_dir, 
      pref, 
      date, 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->kp_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->ki_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->kd_inp)), NULL), 
      data->tag);

  glob_t glob_res;
  glob((const char *)pattern, GLOB_NOSORT, NULL, &glob_res);
  free(pattern);

  char *logpath = calloc(256, sizeof(char));
  sprintf(logpath, 
      "%s/%s_%s(%u)_SP%f-KP%f-KI%f-KD%f_%s.csv", 
      log_dir, 
      pref, 
      date, 
      (unsigned int)glob_res.gl_pathc, 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->kp_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->ki_inp)), NULL), 
      strtof(gtk_entry_get_text(GTK_ENTRY(data->kd_inp)), NULL), 
      data->tag);
  free(date);
  
  data->logpath = logpath;
  gtk_label_set_text(GTK_LABEL(data->logname_lbl), logpath);
  return logpath;
}



int cancel_log(struct Data *data)
{
  if (log_worker_status < THREAD_CANCELLED) {
    log_worker_status = THREAD_CANCELLED;
    g_thread_join(log_worker_thread);
    return 1;
  }

  return 0;

}
