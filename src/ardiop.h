#pragma once

int ard_openserial();
int ard_readserial_until(int fd, char* buf, char until, int buf_max, int timeout);
int ard_readserial_line(int fd, char* buf, int buf_max, int timeout);
int ard_writeserial(int fd, char* buf, int buf_max);
