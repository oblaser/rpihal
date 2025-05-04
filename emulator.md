# Emulator

The emulator allows the project to be run on a host/working machine.

## Usage
The left and right mouse button can be used to click (push a button) or toggle an input pin.

## Build
To build the emulator project include all the same files as in the main project, plus the [emu.cpp](./src/emu/emu.cpp)
file (don't include any other rpihal source file to the build, nor link to the rpihal), and define `RPIHAL_EMU` on
compiler level (not anyehere in source nor header files).

> To create a new emu project in Visual Studio use the _New > Project From Existing Code..._ funtion and create an
> empty console application. Then add all the needed source files described above and add the include directories.

See the [rpihal-system-test](https://github.com/oblaser/rpihal-system-test) for reference.

### Dependencies
[PGE](#credits) hase some dependencies.

#### Ubuntu
To build the emulator the following packages need to be installed, so that the required headers are available.
```sh
sudo apt-get install libx11-dev libmesa-dev libpng-dev
```

## Credits
The emulator uses the OneLoneCoder [Pixel Game Engine](https://github.com/OneLoneCoder/olcPixelGameEngine) for the graphics.
