#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>


#include "ardiop.h"
#include "serial.h"
#include "error.h"
#include "data.h"




void send_key_value_to_arduino(
    struct Data *data, 
    const char *key, 
    void *val_vptr, 
    int type)
{
  char value[(ARDUINO_MESG_LEN/2) + 1] = {0};

  if (strlen(key) > ARDUINO_MESG_LEN/2) {
    timestamp_error(data, 
        "KEY TOO LARGE TO SEND (must be < %d chars, is %lu chars)", 
        ARDUINO_MESG_LEN/2, 
        strlen(key));
    return;
  }

  switch(type) {
    case T_STR:
      if (strlen((char *)val_vptr) > ARDUINO_MESG_LEN/2) {
        timestamp_error(data, 
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

  timestamp(NULL, "sending k/v pair: %s", mesg);
  write(data->serial_fd, mesg, ARDUINO_MESG_LEN);

  switch (wait_for(data, "OK", 10)) {
  case -1:
    timestamp_error(NULL, "Cancelled by user.");
    return;
  case -2:
    timestamp_error(NULL, "Arduino didn't understand message");
    // TODO: deal with this properly
    exit(1);
  }

  timestamp(data, "Sent data { %s = %s } successfully.", key, value);

}
