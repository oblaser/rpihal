> Just some background info useful during development

- https://elinux.org/RPi_GPIO_Code_Samples
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#general-purpose-io-gpio
- https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf + https://elinux.org/BCM2835_datasheet_errata
- http://www.scribd.com/doc/101830961/GPIO-Pads-Control2
- https://forums.raspberrypi.com/viewtopic.php?t=243166


# SPI, I2C, ...

reading `/boot/firmware/overlays/README` is very intresting...

see descriptions for `spiX-Ycs`, X is the index of the SPI module (e.g. `spi0-0cs`).

