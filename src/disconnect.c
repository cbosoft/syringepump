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

    write_serial(data, "QUIT", 4);

    close_serial(data);

    timestamp(data, is_gui, "Closed serial connection");

  }

  form_set_sensitive(data, FORM_DISCONNECTED);

}
