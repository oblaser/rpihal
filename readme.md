# raspihal

A Raspberry Pi Hardware Abstraction Layer for C/C++ projects.

> Not completed. Grows as I need it.

The system tests can also be considered as examples :wink:



## GPIO Module

> __CAUTION!__ Currently only supports models with the BCM2836, BCM2837 and BCM2837B0. _~~BCM283x. Be aware of the different header pinouts on the various boards!~~_
>
> Search for _DEVSPEC_ comments in code to find the critical portions.

### Supported Models:
`2B`
`3A+`
`3B`
`3B+`

These all have the 40 pin header, which has the same pinout on all models listed above.

### Links

- https://elinux.org/RPi_GPIO_Code_Samples
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#general-purpose-io-gpio
- https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf + https://elinux.org/BCM2835_datasheet_errata
- http://www.scribd.com/doc/101830961/GPIO-Pads-Control2
- https://forums.raspberrypi.com/viewtopic.php?t=243166



## Emulator

The emulator can be used to run your project on your working machine (Windows or Linux). To build the emulator project
include all the same files as in the main project, plus the [emu.cpp](src/emu/emu.cpp) file (don't include any other
rpihal source file to the build, nor link to the rpihal).

The emulator uses the OneLoneCoder [Pixel Game Engine](https://github.com/OneLoneCoder/olcPixelGameEngine) for the graphics.
