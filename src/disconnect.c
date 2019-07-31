#include "disconnect.h"
#include "connect.h"
#include "refresh.h"
#include "log.h"
#include "serial.h"
#include "error.h"




void disconnect(struct Data *data, int is_gui)
{
  cancel_connect(data);
  cancel_log(data);
  cancel_refresh(data);

#ifndef WINDOWS
  if (data->serial_fd > 0) {
    write_serial(data, "QUIT", 4);
    close(data->serial_fd);
  }
#else
  //TODO
  // destroy HANDLE appropriately
#endif

  timestamp(data, "Stopped");
}
