# Syringe pump v0.2 alpha

This software controls a syringe pump (device used to push fluid through a
channel at a closely controlled flow rate and stress). The device was originally
intended for medicine delivery, however it is now used for rheological testing.

# Installation

Download a zip file of a release from the [releases page](https://github.com/cbosoft/syringepump/releases)

Then installation is as simple as unzipping in your preferred location! Doesn't
have to be anywhere special. All the zip contains is the program itself, an XML
file which contains the user interface, and this readme file.

# Build from source

Alternatively, you can download this repo and build from source.

## Linux

Simple as

```bash
git clone https://github.com/cbosoft/syringepump
cd syringepump
make
```

to build the UI.

Building the firmware requires the Arduino IDE, or `arduino-builder` with the
`avr-gcc` toolchain. The ArchLinux wiki has a good [section](https://wiki.archlinux.org/index.php/Arduino#Arduino-Builder)
on this.

## MacOS

For MacOS, you'll need the xcode toolchain:

```bash
xcode-select --install
```

and you'll need to install the `gtk+3.0` libraries. I used `homebrew` for this.

```bash
brew install gtk+3.0
```

The `Makefile` uses `pkg-config` to find where the `gtk+` etc libs are
installed, so we need to install `pkg-config` too.

```bash
brew install pkg-config
```

Finally, you will need to add the pkg-config directory for the installed libs to
path. See this [stackoverflow post](https://stackoverflow.com/questions/20098862/how-to-install-gtk-on-osx-for-use-with-g-gcc-compiler).

Add the following to your `.bashrc`:

```bash
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/usr/local/Cellar/cairo/<VERSION>/lib/pkgconfig
```

Change `<VERSION>` to the relevant version on your system.

## Windows

I've tried to get this to compile under Windows usings `mingw64` and `MSYS2`,
but I keep running into inexplicable errors. So building under Windows is not
yet supported.
