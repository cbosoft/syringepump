#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>


#include "serial.h"
#include "error.h"




void get_serial_name(GtkComboBoxText *serial_paths_cmb, GtkWidget *connect_btn)
{
  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  if (!d) {
    timestamp("error reading /dev/*");
    return;
  }

  int count = 0;

  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "tty") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(serial_paths_cmb, path);
      count ++;

    }
  }

  if (!count) {
    timestamp("No arduino found!");
    gtk_widget_set_sensitive(connect_btn, 0);
  }
}
