#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>


#include "serial.h"
#include "error.h"




void refresh_serial_list(struct Data *data)
{
  timestamp(data, "Searching for Arduino...");
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(data->serial_cmb));

  DIR *d;
  struct dirent *dir;
  d = opendir("/dev/.");
  if (!d) {
    timestamp(data, "Error reading /dev/*");
    return;
  }

  int count = 0;

  while ((dir = readdir(d)) != NULL) {
    if (strstr(dir->d_name, "ttyACM") != NULL) {

      char path[261] = {0};
      sprintf(path, "/dev/%s", dir->d_name);

      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->serial_cmb), path);
      count ++;

    }
  }

  if (!count) {
    timestamp(data, "No arduino found!");

    // disable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 0);
  }
  else {

    // enable connect button
    gtk_widget_set_sensitive(GTK_WIDGET(data->conn_btn), 1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(data->serial_cmb), 0);
    if (count == 1) {
      timestamp(data, "Arduino found!");
    }
    else {
      timestamp(data, "Multiple possible Arduino found.");
    }
  }
}

void send_key_value_to_arduino(struct Data *data, const char *key, void *val_vptr, int type)
{
  char value[(ARDUINO_MESG_LEN/2) + 1] = {0};

  if (strlen(key) > ARDUINO_MESG_LEN/2) {
    timestamp(data, 
        "KEY TOO LARGE TO SEND (must be < %d chars, is %lu chars)", 
        ARDUINO_MESG_LEN/2, 
        strlen(key));
    return;
  }

  switch(type) {
    case T_STR:
      if (strlen((char *)val_vptr) > ARDUINO_MESG_LEN/2) {
        timestamp(data, 
            "VALUE TOO LARGE TO SEND (must be < %d chars, is %lu chars)", 
            ARDUINO_MESG_LEN/2,
            strlen((char *)val_vptr));
        return;
      }
      strcpy(value, (char*)val_vptr);
      break;
    case T_FLOAT:
      snprintf(value, ARDUINO_MESG_LEN/2, "%f", CAST(val_vptr, float));
      break;
    case T_INT:
      snprintf(value, ARDUINO_MESG_LEN/2, "%d", CAST(val_vptr, int));
      break;
  }

  char mesg[ARDUINO_MESG_LEN+1] = {0};
  strcat(mesg, key);
  strcat(mesg, "=");
  strcat(mesg, value);

  timestamp(data, "sending k/v pair: %s", mesg);
  write(data->serial_fd, mesg, ARDUINO_MESG_LEN);

  // give the Arduino time to read the full string
  sleep(2);

}
