#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ardiop.h"

int ard_openserial(const char *serial_port_path)
{
  int fd;

  fd = open(serial_port_path, O_RDWR | O_NOCTTY);

  if (fd < 0)
    return -1;

  struct termios toptions;
  tcgetattr(fd, &toptions);
  cfsetispeed(&toptions, B9600);
  cfsetospeed(&toptions, B9600);
  
  // prepare serial for input from arduino (https://chrisheydrick.com/2012/06/17/how-to-read-serial-data-from-an-arduino-in-linux-with-c-part-3/)
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;
  toptions.c_cflag &= ~CRTSCTS;
  toptions.c_cflag |= CREAD | CLOCAL;
  toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
  toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  toptions.c_oflag &= ~OPOST;

  toptions.c_cc[VMIN] = 1;
  toptions.c_cc[VTIME] = 0;

  tcsetattr(fd, TCSANOW, &toptions);

  // give the arduino a second to catch up
  usleep(1000 * 1000);

  //flush stream
  tcflush(fd, TCIFLUSH);

  return fd;
}



int ard_readserial_until(
    int fd, 
    char* buf, 
    char until, 
    int buf_max, 
    int timeout)
{
  char b[1];
  int i = 0;

  do {
    int n = read(fd, b, 1);
    
    if(n == -1)
      return -1;

    if(n == 0) {
      usleep(1000);  // wait 1 msec try again
      timeout--;
      if(timeout == 0)
        return -2;

      continue;
    }
    
    buf[i] = b[0];

    i++;

  } while( b[0] != until && i < buf_max);

  buf[i] = 0;
  return 0;
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
  write(fd, mesg, mesglen);
  return 0;
}
