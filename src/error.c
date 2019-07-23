#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "error.h"
#include "log.h"
#include "data.h"

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

  if (data != NULL) {
    char s[1000] = {0};
    sprintf(s, "<tt><span foreground=\"white\" background=\"red\">%s <b>%s</b></span></tt>", timestr, mesg);
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

  if (IS_EMPTY_ENTRY(data->setpoint_inp)) {
    rv = 1;
    timestamp_error(data, "setpoint cannot be empty");
  }
  
  if (IS_EMPTY_ENTRY(data->kp_inp)) {
    rv = 1;
    timestamp_error(data, "KP cannot be empty");
  }
  
  if (IS_EMPTY_ENTRY(data->ki_inp)) {
    rv = 1;
    timestamp_error(data, "KI cannot be empty");
  }

  if (IS_EMPTY_ENTRY(data->kd_inp)) {
    rv = 1;
    timestamp_error(data, "KD cannot be empty");
  }

  if (IS_EMPTY_ENTRY(data->tag_inp)) {
    rv = 1;
    timestamp_error(data, "tag cannot be empty");
  }

  // tag has no requirements other than to exist.
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
