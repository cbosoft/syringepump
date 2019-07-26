#pragma once
#include "data.h"

#ifdef WINDOWS
#include <Windows.h>
#endif

int wait_for(struct Data *data, const char *trigger, int number);

#ifdef WINDOWS
int ard_openserial(const char *serial_path, HANDLE * serial_handle_ptr);
int ard_closeserial(HANDLE serial_handle);
#else
int ard_openserial(const char *serial_path, int * serial_fd_ptr);
int ard_closeserial(int serial_fd);
#endif

int ard_readserial_until(int fd, char* buf, char until, int buf_max, int timeout);
int ard_readserial_line(int fd, char* buf, int buf_max, int timeout);
int ard_writeserial(int fd, char* buf, int buf_max);

// vim: ft=c
