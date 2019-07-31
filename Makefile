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


gui: syringepump

bumpver:
	touch src/version.h

firmware: build upload

build: $(TARGET)/$(TARGET).ino
	mkdir -p $(BUILD)
	arduino-builder -fqbn $(FQBN) -hardware $(HARDWARE) -tools $(TOOLS) -verbose -build-path $(BUILD) $(TARGET)

upload: build
	$(TOOLS)/avrdude -c$(PROG) -C$(CONFIG) -v -p$(PARTNO) -P$(PORT) -b$(BAUD) -D -Uflash:w:$(BUILD)/$(TARGET).ino.hex:i

clean:
	rm -rf build
	rm -rf src/*.o

CC    = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
LINK   = $(shell pkg-config --libs gtk+-3.0)

OSTR     =
DEFS     :=
ifeq ($(OS),Windows_NT)
 DEFS += -D WINDOWS
 OSTR = win
else
 UNAME_S := $(shell uname -s)
 ifeq ($(UNAME_S),Linux)
  DEFS += -D LINUX
	OSTR = linux
	LINK += "-lX11"
 else
  DEFS += -D OSX
	OSTR = osx
 endif
endif

DEFS += -DDATE=\"$(shell date +"%Y-%m-%d")\"
DEFS += -DARCH=\"$(shell gcc -dumpmachine)\"


OBJ    = src/main.o \
				 src/callbacks.o \
				 src/error.o \
				 src/serial.o \
				 src/log.o \
				 src/connect.o \
				 src/disconnect.o \
				 src/refresh.o \
				 src/form.o \
				 src/util.o
HDRS   = src/data.h

echodefs:
	@echo $(DEFS)

src/%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) $< -c -o $@ $(DEFS)

syringepump: echodefs touchmain $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(DEFS) $(LINK)

touchmain:
	touch src/main.c

release: syringepump
	zip syringepump_$(OSTR).zip syringepump gui/main.ui README.md
