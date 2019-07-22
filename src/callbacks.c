#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "callbacks.h"
#include "ardiop.h"
#include "error.h"
#include "log.h"

extern int LOG_STOPPED;
extern pthread_t log_thread;
struct thread_data td;



void cb_connect(GObject *obj, struct connect_data *cd) {

  timestamp("connecting to \"%s\"", cd->serial_path);
  cd->serial_fd = ard_openserial(cd->serial_path);

  if (cd->serial_fd < 0) {
    timestamp("connecting to \"%s\"", cd->serial_path);

    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(cd->main_win), 
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, 
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_CLOSE, 
        "Could not connect on \"%s\"\n(%d) %s", 
        cd->serial_path, 
        errno, 
        strerror(errno));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  // start log thread
  td.serial_fd = cd->serial_fd;
  td.log_lbl = cd->log_lbl;
  td.scroll = cd->scroll;
  pthread_create(&log_thread, NULL, log_update, &td);

  gtk_widget_set_visible(GTK_WIDGET(cd->conn_btn), 0);
  gtk_widget_set_visible(GTK_WIDGET(cd->disconn_btn), 1);

}




void cb_disconnect(GObject *obj, struct connect_data *cd) {

  LOG_STOPPED = 1;
  pthread_join(log_thread, NULL);

  gtk_widget_set_visible(GTK_WIDGET(cd->conn_btn), 1);
  gtk_widget_set_visible(GTK_WIDGET(cd->disconn_btn), 0);

  close(cd->serial_fd);

  timestamp("disconnected");

}



void cb_quit(GObject *obj, struct connect_data *cd) {
  
  timestamp("closing...");

  if (!LOG_STOPPED)
    cb_disconnect(NULL, cd);

  gtk_main_quit();

}




struct connect_data *create_cd(
    int serial_fd, 
    char *serial_path, 
    int res, 
    GObject *main_win, 
    GObject *conn_btn, 
    GObject *disconn_btn, 
    GObject *log_lbl,
    GObject *scroll)
{
  struct connect_data * cd = malloc(sizeof(struct connect_data));
  
  cd->serial_fd = -1;
  cd->serial_path = "/dev/ttyACM0";
  cd->res = 0;
  cd->main_win = main_win;
  cd->conn_btn = conn_btn;
  cd->disconn_btn = disconn_btn;
  cd->log_lbl = log_lbl;
  cd->scroll = scroll;

  return cd;
}