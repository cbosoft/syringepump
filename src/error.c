#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "error.h"
#include "log.h"

void timestamp(struct connect_data *cd, const char *fmt, ...)
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

  if (cd != NULL) {
    append_text_to_log(cd, timestr);
    append_text_to_log(cd, " - ");
    append_text_to_log(cd, mesg);
  }
}
