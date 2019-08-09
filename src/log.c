#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "log.h"
#include "error.h"
#include "data.h"
#include "form.h"
#include "threads.h"
#include "disconnect.h"
#include "serial.h"
#include "error.h"




static char *log_string = NULL;
static GThread *log_worker_thread;





static void *log_worker(void *void_data)
{ 
  struct Data *data = (struct Data *)void_data;
  data->log_worker_status = THREAD_STARTED;
  
  timestamp(data, 0, "Waiting for Arduino...");
  switch (wait_for(data, 0, "START", 100, &data->log_worker_status, THREAD_CANCELLED)) {
    case -1:
      timestamp_error(data, 0, "Cancelled by user.");
      return NULL;
    case -2:
      timestamp_error(data, 0, "Arduino connection timed out!");
      return NULL;
  }
  timestamp(data, 0, "Arduino ready, starting!");

  get_new_log_name(data, NULL);
  FILE *fp = fopen(data->logpath, "w");
  fclose(fp);

  int charno = 0, timeout = 1000;
  
  struct timespec ms_span;
  ms_span.tv_sec = 0;
  ms_span.tv_nsec = 1000*1000;

  time_t prev_print = 0, just_now;

  while (data->log_worker_status < THREAD_CANCELLED) {
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

    } while(b[0] != '\n' && charno < 512 && (data->log_worker_status < THREAD_CANCELLED));


    received_text[charno-1] = 0;

    if (strcmp(received_text, "STOP") == 0) {
      // arduino requests stop
      timestamp(data, 0, "Arduino finished!");
      data->log_worker_status = THREAD_STOPPED;
      break;
    }


    // write to log file
    fp = fopen(data->logpath, "a");
    fprintf(fp, "%s\n", received_text);
    fclose(fp);

    time(&just_now);

    if (difftime(just_now, prev_print) > 1.0) {
      timestamp(data, 0, "%s", received_text);
      time(&prev_print);
    }
    else {
      timestamp(NULL, 0, "%s", received_text);
    }

    // using third column (position) to get progress
    // must be done last as strtok does funny things to strings
    char *position_s = strtok(received_text, ",");
    for (int i = 0; i < 2; i++) position_s = strtok(NULL, ",");

    if (position_s != NULL) {
      double fraction = 112.0 - atof(position_s);
      form_set_progress(data, fraction);
    }

  }

  if (data->log_worker_status > THREAD_CANCELLED)
    disconnect(data, 0);

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




char *get_new_log_name(struct Data *data, int *control_type_override)
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

  const char *tag = gtk_entry_get_text(GTK_ENTRY(data->tag_inp));
  int taglen = strlen(tag);
  char santag[taglen+1];
  for (int i = 0; i < taglen; i++) {
    if (tag[i] == ' ' || tag[i] == '_' || tag[i] == '.') {
      santag[i] = '-';
    }
    else {
      santag[i] = tag[i];
    }
  }
  santag[taglen] = 0;
  
  if (data->tag == NULL)
    data->tag = malloc((taglen+1)*sizeof(char));
  else
    data->tag = realloc(data->tag, (taglen+1)*sizeof(char));

  strcpy(data->tag, santag);

  char *pattern = calloc(256, sizeof(char));
  int control_type = (control_type_override == NULL) ? (gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab))) : (*control_type_override);
  switch (control_type) {
    case 0: // PID CONTROL
      sprintf(pattern, 
        "%s/%s_%s(*)_SP=%.3f-KP=%.3f-KI=%.3f-KD=%.3f_%s.csv", 
        log_dir, 
        pref, 
        date, 
        strtof(gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)), NULL), 
        strtof(gtk_entry_get_text(GTK_ENTRY(data->kp_inp)), NULL), 
        strtof(gtk_entry_get_text(GTK_ENTRY(data->ki_inp)), NULL), 
        strtof(gtk_entry_get_text(GTK_ENTRY(data->kd_inp)), NULL), 
        data->tag);
      break;
    case 1: // NO CONTROL
      sprintf(pattern, 
        "%s/%s_%s(*)_DC=%.3f_%s.csv", 
        log_dir, 
        pref, 
        date, 
        strtof(gtk_entry_get_text(GTK_ENTRY(data->dc_inp)), NULL), 
        data->tag);
      break;
    default:
      timestamp_error(data, 0, "get_new_log_name", "unrecognised control type");
  }

  glob_t glob_res;
  glob((const char *)pattern, GLOB_NOSORT, NULL, &glob_res);
  free(pattern);

  char *logpath = calloc(256, sizeof(char));
  switch (control_type) {
    case 0: // PID CONTROL
      sprintf(logpath, 
          "%s/%s_%s(%u)_SP=%.3f-KP=%.3f-KI=%.3f-KD=%.3f_%s.csv", 
          log_dir, 
          pref, 
          date, 
          (unsigned int)glob_res.gl_pathc, 
          strtof(gtk_entry_get_text(GTK_ENTRY(data->setpoint_inp)), NULL), 
          strtof(gtk_entry_get_text(GTK_ENTRY(data->kp_inp)), NULL), 
          strtof(gtk_entry_get_text(GTK_ENTRY(data->ki_inp)), NULL), 
          strtof(gtk_entry_get_text(GTK_ENTRY(data->kd_inp)), NULL), 
          data->tag);
      break;
    case 1: // NO CONTROL
      sprintf(logpath, 
          "%s/%s_%s(%u)_DC=%.3f_%s.csv", 
          log_dir, 
          pref, 
          date, 
          (unsigned int)glob_res.gl_pathc, 
          strtof(gtk_entry_get_text(GTK_ENTRY(data->dc_inp)), NULL), 
          data->tag);
      break;
    default:
      timestamp_error(data, 0, "get_new_log_name", "unrecognised control type");
  }
  free(date);
  
  data->logpath = logpath;
  gtk_label_set_text(GTK_LABEL(data->logname_lbl), logpath);
  return logpath;
}



int cancel_log(struct Data *data)
{

  if (data->log_worker_status < THREAD_CANCELLED && data->log_worker_status > THREAD_NULL) {
    data->log_worker_status = THREAD_CANCELLED;
    g_thread_join(log_worker_thread);
    return 1;
  }

  return 0;

}
