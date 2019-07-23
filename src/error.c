#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "error.h"
#include "log.h"
#include "data.h"

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
    append_text_to_log(data, timestr);
    append_text_to_log(data, " - ");
    append_text_to_log(data, mesg);
  }
}
