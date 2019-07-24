## ARDUINO BUILD OPTIONS
FQBN 			= archlinux-arduino:avr:uno
HARDWARE	=	/usr/share/arduino/hardware
TOOLS			=	/usr/bin
BUILD			= build
TARGET		= spfw

# UPLOAD OPTIONS
CONFIG		= /etc/avrdude.conf
PARTNO		= atmega328p
PROG			= arduino
PORT			= /dev/ttyACM0
BAUD			= 115200

build: $(TARGET)/$(TARGET).ino
	mkdir -p $(BUILD)
	arduino-builder -fqbn $(FQBN) -hardware $(HARDWARE) -tools $(TOOLS) -verbose -build-path $(BUILD) $(TARGET)

upload: build
	$(TOOLS)/avrdude -c$(PROG) -C$(CONFIG) -v -p$(PARTNO) -P$(PORT) -b$(BAUD) -D -Uflash:w:$(BUILD)/$(TARGET).ino.hex:i

clean:
	rm -rf build

CC    = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
LINK   = $(shell pkg-config --libs gtk+-3.0)

OSFLAG     :=
ifeq ($(OS),Windows_NT)
 OSFLAG += -D WIN32
 ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
  OSFLAG += -D AMD64
 endif
 ifeq ($(PROCESSOR_ARCHITECTURE),x86)
  OSFLAG += -D IA32
 endif
else
 UNAME_S := $(shell uname -s)
 ifeq ($(UNAME_S),Linux)
  OSFLAG += -D LINUX
	LINK += "-lX11"
 endif
 ifeq ($(UNAME_S),Darwin)
  OSFLAG += -D OSX
 endif
  UNAME_P := $(shell uname -p)
 ifeq ($(UNAME_P),x86_64)
  OSFLAG += -D AMD64
 endif
  ifneq ($(filter %86,$(UNAME_P)),)
 OSFLAG += -D IA32
  endif
 ifneq ($(filter arm%,$(UNAME_P)),)
  OSFLAG += -D ARM
 endif
endif


OBJ    = src/main.o \
				 src/ardiop.o \
				 src/callbacks.o \
				 src/error.o \
				 src/serial.o \
				 src/log.o
HDRS   = src/data.h

src/%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) $< -c -o $@ $(OSFLAG)

syringepump: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(OSFLAG) $(LINK)
