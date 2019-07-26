#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifdef WINDOWS
#include <Windows.h>
#else // Linux or MacOS
#include <termios.h>
#endif

#include "ardiop.h"
#include "data.h"
#include "error.h"
#include "util.h"




int wait_for(struct Data *data, const char *trigger, int timeout_s) {
  
  int delay_us = 500 * 1000; // half a second
  int timeout_n = (1000 * 1000 * timeout_s) / delay_us;

  for (int time = 0; time < timeout_n; time++) {
    char buffer[512] = {0};

    ard_readserial_line(
        data->serial_fd, 
        buffer, 
        512, 
        10);

    if (strcmp(buffer, trigger) == 0)
      return 0;

    ptble_usleep(delay_us);

    timestamp(NULL, 
        "waiting for \"%s\" (%ds / %ds)", 
        trigger, 
        time+1, 
        timeout_n);

  }

  return 1;
}



#ifdef WINDOWS

// https://www.xanthium.in/Serial-port-Programming-using-win32-api
int ard_openserial(const char *serial_port_path, HANDLE * serial_handle_ptr)
{
  HANDLE serial_handle = CreateFile(serial_port_path,
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      0,
      NULL);

  if (serial_handle == INVALID_HANDLE_VALUE)
    return -1;

  DCB serial_params = { 0 };
  serial_params.DCBlength = sizeof(serial_params);

  GetCommState(serial_handle, &serial_params);

  // 9600 8N1
  serial_params.BaudRate = CBR_9600;
  serial_params.ByteSize = 8;
  serial_params.parity = NOPARITY;
  serial_params.StopBits = ONESTOPBIT;

  if (!SetCommState(serial_handle, &serial_params)) {
    CloseHandle(serial_handle);
    return -2;
  }

  // timeout settings
  COMMTIMEOUTS serial_timeouts = { 0 };
  serial_timeouts.ReadIntervalTimeout = 50;
  serial_timeouts.ReadTotalTimeoutConstant = 50;
  serial_timeouts.ReadTotalTimeoutMultiplier = 10;
  serial_timeouts.WriteTotalTimeoutConstant = 50;
  serial_timeouts.WriteTotalTimeoutMultiplier = 10;

  if (!SetCommTimeouts(serial_handle, &serial_timeouts)) {
    CloseHandle(serial_handle);
    return -3;
  }
  
  (*serial_handle_ptr) = serial_handle;
  return 0;
}




int ard_closeserial(HANDLE serial_handle)
{
  return !CloseHandle(serial_handle);
}




int ard_readserial_until(
    HANDLE serial_handle, 
    char* buf, 
    char until, 
    int buf_max, 
    int timeout)
{
  // wait for arduino to start sending data
  SetCommMask(serial_handle, EV_RXCHAR);

  DWORD event_dat;
  if (!WaitCommEvent(serial_handle, &event_data, NULL))
    return -1;

  char ch;
  int rx_bytes_count = 0;
  do {
    ReadFile(serial_handle, &ch, sizeof(char), &rx_bytes_count, NULL);

    if (ch == until)
      return 0;
    else
      buf[rx_bytes_count] = ch;

    rx_bytes_count ++;

  } while (rx_bytes_count > 0 && rx_bytes_count < buf_max);

  return -2;
}




int ard_readserial_line(
    HANDLE serial_handle, 
    char *buf, 
    int buf_max, 
    int timeout)
{
  return ard_readserial_until(serial_handle, buf, '\n', buf_max, timeout);
}




int ard_writeserial(HANDLE serial_handle, char *mesg, int mesglen)
{
  int written = 0;
  int resp = !WriteFile(serial_handle, mesg, mesglen, &written);

  if (!resp)
    return -1;

  if (written != mesglen)
    return -1;

  return 0;
}







#else


int ard_openserial(const char *serial_port_path, int * serial_fd_ptr)
{
  int fd;

  fd = open(serial_port_path, O_RDWR | O_NOCTTY);

  if (fd < 0)
    return -1;

  // prepare serial for input from arduino 
  // https://chrisheydrick.com/2012/06/17/how-to-read-serial-data-from-an-arduino-in-linux-with-c-part-3/
  
  // man 3 termios
  struct termios toptions;
  tcgetattr(fd, &toptions);

  // 9600 BAUD
  cfsetispeed(&toptions, B9600);
  cfsetospeed(&toptions, B9600);

  // control options
  toptions.c_cflag &= ~PARENB; // no parity
  toptions.c_cflag &= ~CSTOPB; // no second stop bit
  toptions.c_cflag &= ~CSIZE; // unset character size
  toptions.c_cflag |= CS8; // set char size to 8
  toptions.c_cflag &= ~CRTSCTS; // hardware flow control
  toptions.c_cflag |= CREAD; // enable receiver
  toptions.c_cflag |= CLOCAL; // ignore modem control lines

  // input options
  toptions.c_iflag &= ~(IXON | IXOFF | IXANY);

  // ?? options
  toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // output options
  toptions.c_oflag &= ~OPOST;
  
  // minimum bytes to read, timeout
  // this combination represents "blocking read"
  //toptions.c_cc[VMIN] = 1;
  //toptions.c_cc[VTIME] = 0;

  // polling read: int read(fd) is non-blocking
  toptions.c_cc[VMIN] = 0;
  toptions.c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSANOW, &toptions)) {
    close(fd);
    return -2;
  }

  // give the arduino a little bit of time to catch up
  usleep(100*1000);

  //flush stream
  tcflush(fd, TCIFLUSH);

  (*serial_fd_ptr) = fd;

  return 0;
}




int ard_closeserial(int serial_fd) {
  return close(serial_fd);
}




int ard_readserial_until(
    int fd, 
    char* buf, 
    char until, 
    int buf_max, 
    int timeout)
{
  char ch[1];
  int i = 0, n;

  do {

    // read until we get something, or connection times out
    while ((n = read(fd, ch, 1)) < 1) {

      if (n < 0 || timeout == 0)
        return -1;

      usleep(1000);
      timeout --;

    }

    if (ch[0] == until)
      return 0;
    else
      buf[i] = ch[0];

    i++;

  } while(i < buf_max-1);

  buf[buf_max-1] = 0;

  // reached end of buffer
  return -2;
}




int ard_readserial_line(
    int fd, 
    char *buf, 
    int buf_max, 
    int timeout)
{
  return ard_readserial_until(fd, buf, '\n', buf_max, timeout);
}




int ard_writeserial(int fd, char *mesg, int mesglen)
{
  int written = 0;
  written = write(fd, mesg, mesglen);

  if (written < 0)
    return -1;

  if (written != mesglen)
    return -2;

  return 0;
}




#endif
