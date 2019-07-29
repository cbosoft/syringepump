#ifdef WINDOWS
#include <windows.h>
#else
#include <time.h>
#endif

#include <string.h>


#include "util.h"


void ptble_usleep(int us)
{
#ifdef WINDOWS
  HANDLE timer;
  LARGE_INTEGER ft;

  ft.QuadPart = -(10*((long long)us));

  timer = CreateWaitableTimer(NULL, TRUE, NULL);
  SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
  WaitForSingleObject(timer, INFINITE);
  CloseHandle(timer);
#else
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = us*1000;
  nanosleep(&ts, NULL);
#endif
}

int is_not_number(const char *s)
{
  int len = strlen(s);
  for (int i = 0; i < len; i++) {
    int c = (int)s[i];

    if ((c < 48 || c > 57) && c != '.' && c != '-')
      return 1;

  }
  return 0;
}
