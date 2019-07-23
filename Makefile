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
OBJ   = src/main.o \
				src/ardiop.o \
				src/callbacks.o \
				src/error.o \
				src/serial.o \
				src/log.o

src/%.o: src/%.c
	$(CC) $(CFLAGS) $< -c -o $@

syringepump: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LINK)
