#include <time.h>
#include <errno.h>
#include <string.h>
#include <glob.h>

#include "log.h"
#include "ardiop.h"
#include "error.h"
#include "data.h"

extern int LOG_STOPPED;
static char *log_string = NULL;




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
  timestamp(data, "Generated log path \"%s\"", logpath);
  return logpath;
}
