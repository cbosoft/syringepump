# ![icon](https://github.com/cbosoft/syringepump/blob/master/icons/Adwaita/64x64/apps/syringepump.png?raw=true) Syringe pump v0.9

![v0.9 screenshot](https://github.com/cbosoft/syringepump/blob/master/screenshots/v0.9.png?raw=true)

This software controls a syringe pump (device used to push fluid through a
channel at a closely controlled flow rate and stress). The device was originally
intended for medicine delivery, however it is now used for rheological testing.

This suite is comprised of the Arduino "firmware" for the syringe pump, the
graphical control software, and a python module (WIP).

Icon is by [bqlqn](https://www.flaticon.com/authors/bqlqn) ([CC 3.0 BY](http://creativecommons.org/licenses/by/3.0/))

# Features

 - Live plot of applied force (N) and flowrate (ml/s).
 - Control a syringe pump's flowrate and applied force using PID.
 - Control parameters can be defined to vary with suspension composition.
 - Step, ramp, constant, sinusoid, and square wave setpoints are supported.
 - CSV output readable by your favourite spreadsheeting/plotting software.

# Build from source

## Linux

Simple as

```bash
git clone https://github.com/cbosoft/syringepump
cd syringepump
make
sudo make install
```

to build and install the main program, and

```bash
make python
```

to build the python module.

Building the firmware requires the [Arduino
IDE](https://www.arduino.cc/en/Main/Software), or `arduino-builder` with the
`avr-gcc` toolchain. The ArchLinux wiki has a good
[section](https://wiki.archlinux.org/index.php/Arduino#Arduino-Builder) on this.

## MacOS and Windows

Windows is a no-go. I've had success compiling on MacOS (Mojave) but I've not
tried recently. Shouldn't require much extra effort than compiling for Linux.
