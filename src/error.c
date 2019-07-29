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
#include "util.h"

#ifdef WINDOWS
#error this file not yet ported (error.h)
#endif

#define CHECK_ENTRY_NUMBER(DATA,MESG_EMPTY,MESG_NOT_NUM) \
  if (IS_EMPTY_ENTRY(DATA)) { \
      rv = 1; \
      timestamp_error(data, MESG_EMPTY); \
    } \
    else { \
      if (is_not_number( gtk_entry_get_text(GTK_ENTRY(DATA)) )) { \
        rv = 1; \
        timestamp_error(data, MESG_NOT_NUM); \
      } \
    }


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

  // setpoint, kp, ki, kd should be numbers
  // (enforced by gtk, but can be overridden using cli args, so do need to 
  // check here)

  if (gtk_notebook_get_current_page(GTK_NOTEBOOK(data->control_tab))) {
    CHECK_ENTRY_NUMBER(data->dc_inp,
        "Duty cycle is a required field for this control scheme.", 
        "Duty cycle must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
  }
  else {
    CHECK_ENTRY_NUMBER(data->setpoint_inp, 
        "Set point is a required field for PID control.", 
        "Set point must be a number (containing only numbers 0-9 and decimal "
        "points ('.').");
    CHECK_ENTRY_NUMBER(data->kp_inp,
        "KP is a required field for PID control.",
        "KP must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->ki_inp,
        "KI is a required field for PID control.",
        "KI must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    CHECK_ENTRY_NUMBER(data->kd_inp,
        "KD is a required field for PID control.",
        "KD must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
  }

  if (IS_EMPTY_ENTRY(data->tag_inp)) {
    rv = 1;
    timestamp_error(data, 
        "Tag should not be empty; use it to describe the run in a few words.");
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
