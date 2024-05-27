> Just some background info useful during development

- https://elinux.org/RPi_GPIO_Code_Samples
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#general-purpose-io-gpio
- https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf + https://elinux.org/BCM2835_datasheet_errata
- http://www.scribd.com/doc/101830961/GPIO-Pads-Control2
- https://forums.raspberrypi.com/viewtopic.php?t=243166


# SPI, I2C, ...

- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#spi-overview

reading `/boot/firmware/overlays/README` is very intresting...
see descriptions for `spiX-Ycs`, X is the index of the SPI module (e.g. `spi0-0cs`).

#### no mod to config.txt
`/dev/` has no SPI devices listed
```
GPFSEL0: 0x00240000
   0: 0b000 IN
   1: 0b000 IN
   2: 0b000 IN
   3: 0b000 IN
   4: 0b000 IN
   5: 0b000 IN
   6: 0b001 OUT
   7: 0b001 OUT
   8: 0b000 IN
   9: 0b000 IN
GPFSEL1: 0x08040000
  10: 0b000 IN
  11: 0b000 IN
  12: 0b000 IN
  13: 0b000 IN
  14: 0b000 IN
  15: 0b000 IN
  16: 0b001 OUT
  17: 0b000 IN
  18: 0b000 IN
  19: 0b001 OUT
GPFSEL2: 0x12049241
  20: 0b001 OUT
  21: 0b000 IN
  22: 0b001 OUT
  23: 0b001 OUT
  24: 0b001 OUT
  25: 0b001 OUT
  26: 0b001 OUT
  27: 0b000 IN
  28: 0b010 AF5
  29: 0b010 AF5
GPFSEL3: 0x3fffffff
  30: 0b111 AF3
  31: 0b111 AF3
  32: 0b111 AF3
  33: 0b111 AF3
  34: 0b111 AF3
  35: 0b111 AF3
  36: 0b111 AF3
  37: 0b111 AF3
  38: 0b111 AF3
  39: 0b111 AF3
GPFSEL4: 0x00000064
  40: 0b100 AF0
  41: 0b100 AF0
  42: 0b001 OUT
  43: 0b000 IN
  44: 0b000 IN
  45: 0b000 IN
  46: 0b000 IN
  47: 0b000 IN
  48: 0b000 IN
  49: 0b000 IN
GPFSEL5: 0x00000000
  50: 0b000 IN
  51: 0b000 IN
  52: 0b000 IN
  53: 0b000 IN
  54: 0b000 IN
  55: 0b000 IN
  56: 0b000 IN
  57: 0b000 IN
```

#### SPI enabled in config.txt
Added `dtoverlay=spi0-0cs` at the top of _config.txt_.

`crw-rw----   1 root    spi    153,   0 May 27 13:48 spidev0.0` is now listed in `/dev/`.

```
   9: 0b100 AF0
GPFSEL1: 0x08040024
  10: 0b100 AF0
  11: 0b100 AF0
```
others same as above.
