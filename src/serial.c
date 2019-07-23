#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>


#include "serial.h"
#include "error.h"




void refresh_serial_list(struct Data *data)
{
  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  if (!d) {
    timestamp(data, "error reading /dev/*");
    return;
  }

  int count = 0;

  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "tty") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->serial_cmb), path);
      count ++;

    }
  }

  if (!count) {
    timestamp(data, "No arduino found!");
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  }
}
