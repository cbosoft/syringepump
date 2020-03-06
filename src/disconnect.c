#include "disconnect.h"
#include "connect.h"
#include "refresh.h"
#include "log.h"
#include "serial.h"
#include "error.h"
#include "form.h"




void disconnect(struct Data *data, int is_gui)
{

  if (cancel_connect(data))
    timestamp(data, is_gui, "Stopped connecting");

  if (cancel_log(data))
    timestamp(data, is_gui, "Stopped logging");

  if (cancel_refresh(data))
    timestamp(data, is_gui, "Stopped refreshing");

  if (is_serial_open(data)) {

    int rv, i = 0;
    while ((rv = write_serial(data, "QUIT", 4)) && ++i < 10);
    if (rv) timestamp_error(data, 1, 1, "Error writing to serial: ");

    close_serial(data);

    timestamp(data, is_gui, "Closed serial connection");

  }

  form_set_sensitive(data, FORM_DISCONNECTED);

}
