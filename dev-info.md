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


## 32bit vs 64bit
- https://raspberrypi.stackexchange.com/a/143293
#### 32bit (2024-03-15-raspios-bookworm-armhf)
```
$ dpkg --print-architecture
armhf
$ file $(command -v file)
/usr/bin/file: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, BuildID[sha1]=6354b71ef1628f57daf8f3326f349e59ccbe65ce, for GNU/Linux 3.2.0, stripped
$ echo | gcc -dM -E [-x c++] - | grep -E "arm|ARM|arch|linux"
#define __ARM_SIZEOF_WCHAR_T 4
#define __ARM_FEATURE_SAT 1
#define __ARM_ARCH_ISA_ARM 1
#define __ARMEL__ 1
#define __ARM_FP 12
#define __linux 1
#define __ARM_SIZEOF_MINIMAL_ENUM 4
#define __gnu_linux__ 1
#define __ARM_PCS_VFP 1
#define __ARM_FEATURE_LDREX 4
#define __ARM_FEATURE_UNALIGNED 1
#define __ARM_FEATURE_QBIT 1
#define __ARM_ARCH_6__ 1
#define __ARM_32BIT_STATE 1
#define __ARM_FEATURE_CLZ 1
#define __ARM_ARCH_ISA_THUMB 1
#define __ARM_ARCH 6
#define linux 1
#define __arm__ 1
#define __ARM_FEATURE_SIMD32 1
#define __ARM_FEATURE_COPROC 15
#define __ARM_FEATURE_DSP 1
#define __ARM_EABI__ 1
#define __linux__ 1
```
#### 64bit (2024-03-15-raspios-bookworm-arm64)
```
$ dpkg --print-architecture
arm64
$ file $(command -v file)
/usr/bin/file: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=d92302f45126662444db75f742f32467503ceefd, for GNU/Linux 3.7.0, stripped
$ echo | gcc -dM -E [-x c++] - | grep -E "arm|ARM|arch|linux"
#define __ARM_SIZEOF_WCHAR_T 4
#define __ARM_FEATURE_IDIV 1
#define __ARM_FP 14
#define __linux 1
#define __ARM_SIZEOF_MINIMAL_ENUM 4
#define __gnu_linux__ 1
#define __ARM_ALIGN_MAX_PWR 28
#define __ARM_FP16_FORMAT_IEEE 1
#define __ARM_FP16_ARGS 1
#define __ARM_FEATURE_CLZ 1
#define __aarch64__ 1
#define __ARM_64BIT_STATE 1
#define __ARM_FEATURE_FMA 1
#define __ARM_ARCH_PROFILE 65
#define __ARM_PCS_AAPCS64 1
#define __ARM_ARCH 8
#define linux 1
#define __ARM_ARCH_8A 1
#define __ARM_FEATURE_UNALIGNED 1
#define __ARM_NEON 1
#define __ARM_ALIGN_MAX_STACK_PWR 16
#define __ARM_FEATURE_NUMERIC_MAXMIN 1
#define __ARM_ARCH_ISA_A64 1
#define __linux__ 1
```



---

# SPI, I2C, ...

The documentation can mainly be found at:
- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#spi-overview

- `/boot/firmware/overlays/README`

## SPI
See descriptions for `spiX-Ycs`, X is the index of the SPI peripheral and Y the number of chip select signal (e.g. `spi0-0cs`) and thus the number of SPI devices listed in _/dev_.

#### Clock not as expected?
- https://github.com/raspberrypi/linux/issues/3381#issuecomment-567536640
- [seems to be OK on Pi5](https://github.com/raspberrypi/linux/issues/3381#issuecomment-1893699525)

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
