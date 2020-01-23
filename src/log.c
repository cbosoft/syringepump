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

void write_run_params(struct Data *data)
{
  FILE *fp = fopen(data->logpath, "w");
  FORM_CONTROL_SELECTION control_type = form_get_control_type(data);
  FORM_SETTER_SELECTION setter_type = form_get_setter_type(data);

  switch (control_type) {

    case FORM_CONTROL_PID:
      fprintf(fp, "PID control\n");
      fprintf(fp, "KP, KI, KD\n");
      fprintf(fp, "%s\n\n", form_get_pid_params(data));
      break;

    case FORM_CONTROL_MEAS:
      fprintf(fp, "Passive control\n");
      fprintf(fp, "Measure Time (s)\n");
      fprintf(fp, "%s\n\n", form_get_meas_params(data));
      break;

    case FORM_CONTROL_NONE:
      fprintf(fp, "Voltage control\n");
      fprintf(fp, "Voltage is set directly according to setter.\n\n");
      var_unit = "V";
      break;
  }
  

  char *s;

  switch (setter_type) {

    case FORM_SETTER_CONSTANT:
      fprintf(fp, "Constant setpoint\n");
      fprintf(fp, "Value\n");
      fprintf(fp, "%s\n", form_get_const_setter_params(data));
      break;

    case FORM_SETTER_RAMP:
      fprintf(fp, "Linearly changing setpoint\n");
      fprintf(fp, "Gradient,Intercept\n");
      s = form_get_ramp_setter_params(data);
      fprintf(fp, "%s\n", s);
      free(s);
      s = NULL;
      break;

    case FORM_SETTER_STEP:
      fprintf(fp, "Linearly changing setpoint\n");
      fprintf(fp, "Initial Value (%s),Time of Change (s),Final Value (%s)\n", var_unit, var_unit);
      s = form_get_step_setter_params(data);
      fprintf(fp, "%s\n", s);
      free(s);
      s = NULL;
      break;

    case FORM_SETTER_SINE:
      fprintf(fp, "Sine wave setpoint\n");
      fprintf(fp, "Frequency,Magnitude,Mean\n");
      s = form_get_sine_setter_params(data);
      fprintf(fp, "%s\n", s);
      free(s);
      s = NULL;
      break;

  }

  fprintf(fp, "Stop buffer (mm), Syringe diameter (mm)\n");
  s = form_get_bldi_data(data);
  fprintf(fp, "%s\n", s);
  free(s);

  fprintf(fp, "\n");
  fclose(fp);
}




//
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
  write_run_params(data);

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
    FILE *fp = fopen(data->logpath, "a");
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
      double prev = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(get_object_safe(data, "progProgress")));
      double fraction = 1.0 - (atof(position_s) / 110.0);
      if (fraction > prev || prev >= 1.0)
        form_set_progress(data, fraction);
    }

  }

  if (data->log_worker_status > THREAD_CANCELLED)
    disconnect(data, 0);

  form_set_progress(data, 1.0);

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

  gtk_label_set_markup(GTK_LABEL(get_object_safe(data, "lblLog")), log_string);

}




char *get_new_log_name(struct Data *data, int *control_type_override)
{
  char *log_dir = gtk_file_chooser_get_current_folder(
      GTK_FILE_CHOOSER(get_object_safe(data, "fchLogFolder")));

  (void) control_type_override;

  const char *pref = "syrpu";

  char *date = calloc(15, sizeof(char));
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(date, 50, "%Y-%m-%d", timeinfo);

  const char *tag = gtk_entry_get_text(GTK_ENTRY(get_object_safe(data, "entTag")));
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
  santag[taglen] = '\0';
  
  data->tag = realloc(data->tag, (taglen+1)*sizeof(char));
  strcpy(data->tag, santag);

  char *pattern = calloc(256, sizeof(char));
  sprintf(pattern, 
    "%s/%s_%s(*)_%s.csv", 
    log_dir, 
    pref, 
    date, 
    data->tag);

  glob_t glob_res;
  glob((const char *)pattern, GLOB_NOSORT, NULL, &glob_res);
  free(pattern);

  char *logpath = calloc(256, sizeof(char));
  sprintf(logpath, 
      "%s/%s_%s(%u)_%s.csv", 
      log_dir, 
      pref, 
      date, 
      (unsigned int)glob_res.gl_pathc, 
      data->tag);
  free(date);
  
  data->logpath = logpath;
  gtk_label_set_text(GTK_LABEL(get_object_safe(data, "lblLogName")), logpath);
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
