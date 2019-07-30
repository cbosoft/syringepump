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
