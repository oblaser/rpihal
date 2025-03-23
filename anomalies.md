# Anomalies

Unexpected behaviour or anomalies are documented here.

#### Index
- [ANOM1 - GPIO alternate function registers](#anom1---gpio-alternate-function-registers)
- [ANOM2 - Multiple SPI instances on the same device](#anom2---multiple-spi-instances-on-the-same-device)



# ANOM1 - GPIO alternate function registers

> _from [elinux.org errata](https://elinux.org/BCM2835_datasheet_errata#p92_to_95_.26_102_to_103):_</br>
> The GPIO Alternate function select Registers. This shows a bit pattern of 111 as alternative function 3. If you look at the values after boot up the Pi's SPI interface pins connected to the SD card have this value in them. This does not match the diagram on page 102 - 103 which shows this function is selected with alternative function 4. So either the bit pattern / function information is wrong or Table 6-31 is wrong.

Maybe [config.txt](https://www.raspberrypi.com/documentation/computers/config_txt.html#gpio) can help to investigate.



# ANOM2 - Multiple SPI instances on the same device

It's possible to open multiple SPI instances (actually file descriptors) on the same device (e.g. `/dev/spidev0.0`). If done so the clock speed is tied to the instance, whereas the configuration is tied to the device. Therefore the configuration of the latest `RPIHAL_SPI_open()` is _applied_ to all instances.

```c
RPIHAL_SPI_instance_t spi_a, spi_b;

RPIHAL_SPI_open(&spi_a, "/dev/spidev0.0",  50000, RPIHAL_SPI_CFG_MODE_0 | RPIHAL_SPI_CFG_NO_CS);
RPIHAL_SPI_open(&spi_b, "/dev/spidev0.0", 300000, RPIHAL_SPI_CFG_MODE_2);

RPIHAL_SPI_transfer(&spi_a, txBuffer, rxBuffer, 2); // asserts CE0, transfers in mode 2 at 50kHz
RPIHAL_SPI_transfer(&spi_b, txBuffer, rxBuffer, 5); // asserts CE0, transfers in mode 2 at 300kHz

RPIHAL_SPI_close(&spi_a);
RPIHAL_SPI_close(&spi_b);
```
