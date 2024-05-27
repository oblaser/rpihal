# Anomalies

Any unexpected behaviour or anomalies are reported here.

#### Index
- [ANOM1](#anom1---gpio-alternate-function-registers)

---

# ANOM1 - GPIO alternate function registers

> **[elinux.org errata](https://elinux.org/BCM2835_datasheet_errata#p92_to_95_.26_102_to_103)**</br>
> The GPIO Alternate function select Registers. This shows a bit pattern of 111 as alternative function 3. If you look at the values after boot up the Pi's SPI interface pins connected to the SD card have this value in them. This does not match the diagram on page 102 - 103 which shows this function is selected with alternative function 4. So either the bit pattern / function information is wrong or Table 6-31 is wrong.

Further investigation needed, WIP...
