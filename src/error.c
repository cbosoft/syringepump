#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
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
#include "form.h"

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

struct timestamp_callback_data {
  struct Data *data;
  char *message;
};


gboolean timestamp_callback(struct timestamp_callback_data *tcd)
{

  append_text_to_log(tcd->data, tcd->message);

  free(tcd->message);
  free(tcd);

  return 0;
}

gboolean status_callback(struct timestamp_callback_data *tcd)
{

  GObject *status_label = get_object_safe(tcd->data, "lblStatus");
  gtk_label_set_markup(GTK_LABEL(status_label), tcd->message);

  free(tcd->message);
  free(tcd);

  return 0;
}




void timestamp_error(struct Data *data, int is_gui, int show_errno, const char *fmt, ...)
{
  (void) is_gui;

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
  //FILE *logf = open_log(log_path);
  //fprintf(logf, "%s - %s\n", timestr, mesg);
  //fclose(logf);

  if (data != NULL) {
    char s[ERROR_BUFFER_SIZE+1] = {0};
    snprintf(s, ERROR_BUFFER_SIZE,
        "<tt>"
        "<span foreground=\"white\" background=\"red\">"
        "<b>%s</b>"
        "</span>"
        "</tt>", mesg);
    
    if (show_errno && errno) {

      char e[(ERROR_BUFFER_SIZE/2)+1] = {0};
      snprintf(e, ERROR_BUFFER_SIZE/2, " (%d) %s", errno, strerror(errno));
      strncat(s, e, ERROR_BUFFER_SIZE);
      errno = 0;

    }
    

    struct timestamp_callback_data *tcd = calloc(1, sizeof(struct timestamp_callback_data));
    tcd->data = data;
    tcd->message = strdup(s);
    if (is_gui)
      status_callback(tcd);
    else {
      g_idle_add((GSourceFunc)status_callback, tcd);
    }
  }
}




// TODO: simplify timestamp by removing GUI printing
void timestamp(struct Data *data, int is_gui, const char *fmt, ...)
{

  (void) is_gui;

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
    char s[ERROR_BUFFER_SIZE] = {0};
    snprintf(s, ERROR_BUFFER_SIZE, "<tt><small>%s <b>%s</b></small></tt>", timestr, mesg);

    // if (is_gui)
    //   append_text_to_log(data, s);
    // else {
    //   struct timestamp_callback_data *tcd = calloc(1, sizeof(struct timestamp_callback_data));
    //   tcd->data = data;
    //   tcd->message = strdup(s);
    //   g_idle_add((GSourceFunc)timestamp_callback, tcd);
    // }
  }
}
