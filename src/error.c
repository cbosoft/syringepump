#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef WINDOWS
#include <errno.h>
#endif

#include "error.h"
#include "log.h"
#include "data.h"

#ifdef WINDOWS
#error this file not yet ported (error.h)
#endif


const char *log_path = "log.txt";

FILE *open_log(const char *path)
{
  FILE *fp = fopen(path, "a");

  if (fp == NULL && errno == ENOENT)
    fp = fopen(path, "w");

  return fp;
}



void timestamp_error(struct Data *data, const char *fmt, ...)
{

  char mesg[MESGLEN] = {0};

  va_list ap;

  va_start(ap, fmt);
  vsnprintf(mesg, MESGLEN-1, fmt, ap);
  va_end(ap);

  time_t rt;
  struct tm *info;
  char timestr[MESGLEN] = {0};

  time(&rt);
  info = localtime(&rt);
  strftime(timestr, MESGLEN-1, "%x %X", info);

  fprintf(stderr, "%s - %s\n", timestr, mesg);
  FILE *logf = open_log(log_path);
  fprintf(logf, "%s - %s\n", timestr, mesg);
  fclose(logf);


  if (data != NULL) {
    char s[1000] = {0};
    sprintf(s, 
        "<tt>"
        "<span foreground=\"white\" background=\"red\">"
        "<small>%s</small> "
        "<b>%s</b>"
        "</span>"
        "</tt>", timestr, mesg);
    
    if (errno) {
      char e[1000] = {0};
      sprintf(e, " (%d) %s", errno, strerror(errno));
      strcat(s, e);
      errno = 0;
    }

    append_text_to_log(data, s);
  }
}




void timestamp(struct Data *data, const char *fmt, ...)
{

  char mesg[MESGLEN] = {0};

  va_list ap;

  va_start(ap, fmt);
  vsnprintf(mesg, MESGLEN-1, fmt, ap);
  va_end(ap);

  time_t rt;
  struct tm *info;
  char timestr[MESGLEN] = {0};

  time(&rt);
  info = localtime(&rt);
  strftime(timestr, MESGLEN-1, "%x %X", info);

  fprintf(stderr, "%s - %s\n", timestr, mesg);
  FILE *logf = open_log(log_path);
  fprintf(logf, "%s - %s\n", timestr, mesg);
  fclose(logf);

  if (data != NULL) {
    char s[1000] = {0};
    sprintf(s, "<tt><small>%s <b>%s</b></small></tt>", timestr, mesg);
    append_text_to_log(data, s);
  }
}




int check_form(struct Data *data)
{
  int rv = 0;

  // check over the form, pre connection.
  // none of the inputs should be empty (setpoint, kp, ki, kd, tag)

  // setpoint, kp, ki, kd should be numbers (single precision floats) 
  // (enforced by gtk)

  if (gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab))) {

    if (IS_EMPTY_ENTRY(data->dc_inp)) {
      rv = 1;
      timestamp_error(data, "Duty cycle is a required field for this control scheme.");
    }

  }
  else {

    if (IS_EMPTY_ENTRY(data->setpoint_inp)) {
      rv = 1;
      timestamp_error(data, "Set point is a required field for PID control.");
    }
    
    if (IS_EMPTY_ENTRY(data->kp_inp)) {
      rv = 1;
      timestamp_error(data, "KP is a required field for PID control. Set to zero to disable.");
    }
    
    if (IS_EMPTY_ENTRY(data->ki_inp)) {
      rv = 1;
      timestamp_error(data, "KI is a reuired field for PID control. Set to zero to disable.");
    }

    if (IS_EMPTY_ENTRY(data->kd_inp)) {
      rv = 1;
      timestamp_error(data, "KD is a required field for PID control. Set to zero to disable.");
    }

    if (IS_EMPTY_ENTRY(data->tag_inp)) {
      rv = 1;
      timestamp_error(data, "Tag should not be empty; use it to describe the run in a few words.");
    }

  }

  // if tag contains spaces, periods, or underscores; they will be 
  // replaced with dashes

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
  gtk_entry_set_text(GTK_ENTRY(data->tag_inp), santag);

  return rv;
}
