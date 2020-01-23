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

#include <dirent.h>
#include <string.h>

#include <gtk/gtk.h>


#include "serial.h"
#include "error.h"
#include "data.h"
#include "form.h"
#include "threads.h"
#include "data.h"
#include "error.h"
#include "util.h"

#define ARDUINO_MESG_LEN 64


void send_data_packet(struct Data *data, int is_gui, const char *key, const char *value)
{
  if (key == NULL) {
    timestamp_error(data, is_gui, "key to send was null. PLEASE LET CHRIS KNOW IF THIS ERROR POPS UP!");
    return;
  }

  if (value == NULL) {
    timestamp_error(data, is_gui, "value to send was null. PLEASE LET CHRIS KNOW IF THIS ERROR POPS UP!");
    return;
  }

  int len;
  if ((len = strlen(key)) > ARDUINO_MESG_LEN/2) {
    timestamp_error(data, is_gui,
        "KEY TOO LARGE TO SEND (must be < %d chars, is %lu chars)", 
        ARDUINO_MESG_LEN/2, 
        len);
    return;
  }

  if ((len = strlen(value)) > ARDUINO_MESG_LEN/2) {
    timestamp_error(data, is_gui,
        "VALUE TOO LARGE TO SEND (must be < %d chars, is %lu chars)", 
        ARDUINO_MESG_LEN/2,
        len);
    return;
  }

  char mesg[ARDUINO_MESG_LEN+1] = {0};
  sprintf(mesg, "%s=%s", key, value);

  timestamp(NULL, is_gui, "sending k/v pair: %s", mesg);
  write(data->serial_fd, mesg, ARDUINO_MESG_LEN);

  switch (wait_for(data, is_gui, "OK", 10, &data->connect_worker_status, THREAD_CANCELLED)) {
  case -1:
    timestamp_error(NULL, is_gui, "Cancelled by user.");
    return;
  case -2:
    timestamp_error(NULL, is_gui, "Arduino didn't understand message");
    // TODO: deal with this properly
    exit(1);
  }

  timestamp(data, is_gui, "Sent data { %s = %s } successfully.", key, value);

}




int wait_for(struct Data *data, int is_gui, const char *trigger, int timeout_s, int *flagaddr, int stopval) {
  
  int delay_us = 10 * 1000;
  int timeout_n = (1000 * 1000 * timeout_s) / delay_us;

  if (trigger == NULL) {
    timestamp(NULL, 0, "NULL TRIGGER");
  }

  if (flagaddr == NULL) {
    timestamp(NULL, 0, "NULL FLAGADDR");
  }

  for (int time = 0; time < timeout_n; time++) {

    char buffer[512] = {0};

    read_serial_line(data, buffer, 512, 10);

    if (strcmp(buffer, trigger) == 0)
      return 0;

    //ptble_usleep(delay_us);

    if ((*flagaddr) == stopval)
      return -1;

    timestamp(NULL, is_gui,
        "waiting for \"%s\" (%ds / %ds)", 
        trigger, 
        (int)( ((float)time+1) * ((float)delay_us) / ((float)timeout_n) ), 
        timeout_s);

  }

  return -2;
}



#ifdef WINDOWS

// https://www.xanthium.in/Serial-port-Programming-using-win32-api
int open_serial(const char *serial_port_path, struct Data *data)
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
  
  data->serial_handle = serial_handle;
  return 0;
}








#else // NOT WINDOWS: MACOS OR LINUX




int open_serial(const char *serial_port_path, struct Data *data)
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
  toptions.c_cc[VTIME] = 2;

  if (tcsetattr(fd, TCSANOW, &toptions)) {
    close(fd);
    return -2;
  }

  // give the arduino a little bit of time to catch up
  usleep(100*1000);

  //flush stream
  tcflush(fd, TCIFLUSH);

  data->serial_fd = fd;

  return 0;
}




int is_serial_open(struct Data *data)
{

#ifdef WINDOWS

  // TODO

#else

  return data->serial_fd > 0;

#endif

}



int close_serial(struct Data *data)
{

#ifdef WINDOWS
  int rv = !CloseHandle(data->serial_handle);
#else
  int rv = close(data->serial_fd);

  if (!rv)
    data->serial_fd = -1;
#endif

  return rv;
}




int read_serial_until(
    struct Data *data, 
    char* buf, 
    int buf_max, 
    char until, 
    int timeout)
{
#ifdef WINDOWS

  // wait for arduino to start sending data
  SetCommMask(serial_handle, EV_RXCHAR);

  DWORD event_dat;
  if (!WaitCommEvent(serial_handle, &event_data, NULL))
    return -1;

#endif

  char ch[1] = {0};
  int rx_bytes_count = 0, n = 0;

  do {
#ifdef WINDOWS
    // todo non blocking (cancelable)
    ReadFile(serial_handle, ch, sizeof(char), &rx_bytes_count, NULL);
#else
    // read until we get something, or connection times out
    while ((n = read(data->serial_fd, ch, 1)) < 1) {

      if (n < 0 || timeout == 0)
        return -1;

      //ptble_usleep(10);
      timeout --;

    }
#endif

  if (ch[0] == until)
    return 0;
  else
    buf[rx_bytes_count] = ch[0];

  rx_bytes_count++;

  } while(rx_bytes_count > 0 && rx_bytes_count < buf_max);

  buf[buf_max-1] = 0;

  // reached end of buffer
  return -2;
}




int read_serial_line(
    struct Data *data, 
    char *buf, 
    int buf_max, 
    int timeout)
{
  return read_serial_until(data, buf, buf_max, '\n', timeout);
}




int write_serial(struct Data *data, char *mesg, int mesglen)
{

  int written = 0;

#ifdef WINDOWS

  int resp = !WriteFile(data->serial_handle, mesg, mesglen, &written);

  if (!resp)
    return -1;

#else

  written = write(data->serial_fd, mesg, mesglen);

  if (written < 0)
    return -1;

#endif


  if (written != mesglen)
    return -2;

  return 0;
}




#endif
