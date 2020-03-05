#pragma once

// <KEY>=<VALUE>
// total length 64 bytes, 1 for separator, 2 for key, 61 for value
#define ARDUINO_MESG_LEN 64
#define PACKET_KEY_LEN 2
#define PACKET_VALUE_LEN (ARDUINO_MESG_LEN - 1 - PACKET_KEY_LEN)
