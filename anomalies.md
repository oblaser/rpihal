# Anomalies

Any unexpected behaviour or anomalies are reported here.

#### Index
- [ANOM1 - GPIO alternate function registers](#anom1---gpio-alternate-function-registers)

---

# ANOM1 - GPIO alternate function registers

> _from [elinux.org errata](https://elinux.org/BCM2835_datasheet_errata#p92_to_95_.26_102_to_103):_</br>
> The GPIO Alternate function select Registers. This shows a bit pattern of 111 as alternative function 3. If you look at the values after boot up the Pi's SPI interface pins connected to the SD card have this value in them. This does not match the diagram on page 102 - 103 which shows this function is selected with alternative function 4. So either the bit pattern / function information is wrong or Table 6-31 is wrong.

Maybe [config.txt](https://www.raspberrypi.com/documentation/computers/config_txt.html#gpio) can help to investigate.

Further investigation needed, WIP... `RPIHAL_GPIO_dumpAltFuncReg()`

#### Raspberry Pi 4 Model B Rev 1.5
| GPIO | ALT0 | ALT1 | ALT2 | ALT3 | ALT4 | ALT5 |
|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
|   0  |      |      |      |      |      |      |
|   1  |      |      |      |      |      |      |
|   2  |      |      |      |      |      |      |
|   3  |      |      |      |      |      |      |
|   4  |      |      |      |      |      |      |
|   5  |      |      |      |      |      |      |
|   6  |      |      |      |      |      |      |
|   7  |      |      |      |      |      |      |
|   8  |      |      |      |      |      |      |
|   9  |  OK  |      |      |      |      |      |
|  10  |  OK  |      |      |      |      |      |
|  11  |  OK  |      |      |      |      |      |
|  12  |      |      |      |      |      |      |
|  13  |      |      |      |      |      |      |
|  14  |      |      |      |      |      |      |
|  15  |      |      |      |      |      |      |
|  16  |      |      |      |      |      |      |
|  17  |      |      |      |      |      |      |
|  18  |      |      |      |      |      |      |
|  19  |      |      |      |      |  OK  |      |
|  20  |      |      |      |      |  OK  |      |
|  21  |      |      |      |      |  OK  |      |
|  22  |      |      |      |      |      |      |
|  23  |      |      |      |      |      |      |
|  24  |      |      |      |      |      |      |
|  25  |      |      |      |      |      |      |
|  26  |      |      |      |      |      |      |
|  27  |      |      |      |      |      |      |
