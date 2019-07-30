#include "disconnect.h"
#include "threads.h"
#include "ardiop.h"
#include "error.h"




extern int connect_worker_status;
extern int log_worker_status;
extern int refresh_worker_status;
extern GThread *connect_worker_thread;
extern GThread *log_worker_thread;
extern GThread *refresh_worker_thread;




void disconnect(struct Data *data)
{
  if (connect_worker_status < THREAD_CANCELLED) {
    connect_worker_status = THREAD_CANCELLED;
    g_thread_join(connect_worker_thread);
  }
  
  if (log_worker_status < THREAD_CANCELLED) {
    log_worker_status = THREAD_CANCELLED;
    g_thread_join(log_worker_thread);
  }
  
  if (refresh_worker_status < THREAD_CANCELLED) {
    refresh_worker_status = THREAD_CANCELLED;
    g_thread_join(refresh_worker_thread);
  }

#ifndef WINDOWS
  ard_writeserial(data->serial_fd, "QUIT", 4);
  if (data->serial_fd > 0)
    close(data->serial_fd);
#else
  //TODO
  // tell arduino to reset, destroy HANDLE appropriately
#endif

  timestamp(data, "disconnected");
}
