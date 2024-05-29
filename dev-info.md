#### just some links
- https://elinux.org/RPi_GPIO_Code_Samples
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#general-purpose-io-gpio
- https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf + https://elinux.org/BCM2835_datasheet_errata
- http://www.scribd.com/doc/101830961/GPIO-Pads-Control2
- https://forums.raspberrypi.com/viewtopic.php?t=243166

- https://www.kernel.org/doc/html/latest/i2c/dev-interface.html
- https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/plain/tools/i2ctransfer.c


#### Model, SoC, ... detection
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-revision-codes
- https://github.com/raspberrypi/documentation/blob/develop/documentation/asciidoc/computers/raspberry-pi/revision-codes.adoc


---

# SPI, I2C, ...

The documentation can mainly be found at:
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#spi-overview

- `/boot/firmware/overlays/README`

## SPI
See descriptions for `spiX-Ycs`, X is the index of the SPI peripheral and Y the number of chip select signal (e.g. `spi0-0cs`) and thus the number of SPI devices listed in _/dev_.

## I2C
> `/boot/firmware/overlays/README`
>
> Note also that i2c, i2c_arm and i2c_vc are aliases for the physical interfaces i2c0 and i2c1. Use of the numeric
> variants is still possible but deprecated because the ARM/VC assignments differ between board revisions. The same
> oard-specific mapping applies to i2c_baudrate, and the other i2c baudrate parameters.
>
> See more detail in `i2c0` info.

See descriptions for `i2cX`, X is the index of the I2C (BSC) peripheral.

Use `i2c-gpio` for bit banged I2C.

> from [raspi forum](https://forums.raspberrypi.com/viewtopic.php?t=154623#p1011384)
>
> `i2c_arm` is an alias to `i2c1` on the majority of Pi's (very early model B's it aliases to `i2c0`).</br>
> `dtparam=i2c_vc=on` = `i2c-0`</br>
> `dtparam=i2c_arm=on` = `i2c-1`</br>
> `dtparam=i2c2_iknowwhatimdoing=on` = `i2c-2`</br>

`$ i2cdetect` might be useful (`-a` for addresses above 0x77).

## on RasPi4B
#### no mod to config.txt
```
$ ls /dev/ | grep -E "i2c|spi"
i2c-20
i2c-21
```

#### SPI and I2C enabled in config.txt
At the top of _config.txt_ added:
```
dtoverlay=spi0-0cs
dtoverlay=i2c1
```

```
$ ls /dev/ | grep -E "i2c|spi"
i2c-1
i2c-20
i2c-21
spidev0.0
```

```
RPIHAL_GPIO_dumpAltFuncReg selected pins: 0x0000'0000'0000'0f8c
GPFSEL0: 0x20040900
   2: 0b100 AF0
   3: 0b100 AF0
   7: 0b000 IN
   8: 0b000 IN
   9: 0b100 AF0
GPFSEL1: 0x08040024
  10: 0b100 AF0
  11: 0b100 AF0

RPIHAL_GPIO_dumpPullUpDnReg selected pins: 0x0000'0000'0000'0f8c
GPIO_PUP_PDN_CNTRL_REG0: 0x4aa94655
   2: 0b01 UP
   3: 0b01 UP
   7: 0b01 UP
   8: 0b01 UP
   9: 0b10 DOWN
  10: 0b10 DOWN
  11: 0b10 DOWN
```
