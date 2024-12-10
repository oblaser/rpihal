https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#check-raspberry-pi-model-and-cpu-across-distributions

#### 2024-03-15-raspios-bookworm-armhf
| Model | `/proc/device-tree/compatible` | `/proc/device-tree/model` |
|:---:|:---|:---|
| B rev 1    | `raspberrypi,model-b`&#x2400;`brcm,bcm2835`        | `Raspberry Pi Model B Rev 1`          |
| B rev 2    | `raspberrypi,model-b`&#x2400;`brcm,bcm2835`        | `Raspberry Pi Model B Rev 2`          |
| B+ rev 1.2 |                                                    | `Raspberry Pi Model B Plus Rev 1.2`   |
| Z          |  |  |
| ZW         |                                                    | `Raspberry Pi Zero W Rev 1.1`         |
| CM         |                                                    | `Raspberry Pi Compute Module Rev 1.0` |
| 2B (pre rev 1.2) |                                              | `Raspberry Pi 2 Model B Rev 1.1`      |
| 2B rev 1.2 |                                                    | `Raspberry Pi 2 Model B Rev 1.2`      |
| 3B         | `raspberrypi,3-model-b`&#x2400;`brcm,bcm2837`      | `Raspberry Pi 3 Model B Rev 1.2`      |
| CM3        |                                                    | `Raspberry Pi Compute Module 3 Rev 1.0` |
| Z2W        | `raspberrypi,model-zero-2-w`&#x2400;`brcm,bcm2837` | `Raspberry Pi Zero 2 W Rev 1.0`       |
| 3A+        | `raspberrypi,3-model-b-plus`&#x2400;`brcm,bcm2837` [^1] | `Raspberry Pi 3 Model A Plus Rev 1.0` |
| 3B+        | `raspberrypi,3-model-b-plus`&#x2400;`brcm,bcm2837` | `Raspberry Pi 3 Model B Plus Rev 1.3` |
| CM3+       |                                                    | `Raspberry Pi Compute Module 3 Plus Rev 1.0` |
| 4B         | `raspberrypi,4-model-b`&#x2400;`brcm,bcm2711`      | `Raspberry Pi 4 Model B Rev 1.1`</br>`Raspberry Pi 4 Model B Rev 1.2`</br>`Raspberry Pi 4 Model B Rev 1.4` |
| 400        | `raspberrypi,400`&#x2400;`brcm,bcm2711`            | `Raspberry Pi 400 Rev 1.0`            |
| CM4        |  |  |
| 5          |                                                    | `Raspberry Pi 5 Model B Rev 1.0`      |


#### 2024-03-15-raspios-bookworm-arm64
| Model | `/proc/device-tree/compatible` | `/proc/device-tree/model` |
|:---:|:---|:---|
| 3B+        | `raspberrypi,3-model-b-plus`&#x2400;`brcm,bcm2837` | `Raspberry Pi 3 Model B Plus Rev 1.4` |
| 4B         | `raspberrypi,4-model-b`&#x2400;`brcm,bcm2711`      | `Raspberry Pi 4 Model B Rev 1.5`      |


#### 2022-04-04 raspbian lite
| Model | `/proc/device-tree/compatible` | `/proc/device-tree/model` |
|:---:|:---|:---|
| 3B         | `raspberrypi,3-model-b`&#x2400;`brcm,bcm2837`      | `Raspberry Pi 3 Model B Rev 1.2` |
| 3B+        | `raspberrypi,3-model-b-plus`&#x2400;`brcm,bcm2837` | `Raspberry Pi 3 Model B Plus Rev 1.3` |


#### 2015-11-21-raspbian-jessie-lite
| Model | `/proc/device-tree/compatible` | `/proc/device-tree/model` |
|:---:|:---|:---|
| B rev 1    | `brcm,bcm2708` | `Raspberry Pi Model B Rev 1` |
| B rev 2    | `brcm,bcm2708` | `Raspberry Pi Model B Rev 2` |



## The Script

```sh
cat /proc/device-tree/compatible | tr '\0' '\n'

echo "##########"

cat /proc/device-tree/model | tr '\0' '\n'

echo "##########"

cat /proc/cpuinfo | grep -E "Hardware|Revision|Serial|Model"
```

one-liner:
```sh
cat /proc/device-tree/compatible | tr '\0' '\n' ; echo "##########" ; cat /proc/device-tree/model | tr '\0' '\n' ; echo "##########" ; cat /proc/cpuinfo | grep -E "Hardware|Revision|Serial|Model"
```



---

- https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-revision-codes
- https://github.com/AndrewFromMelbourne/raspberry_pi_revision
- https://gist.github.com/jperkin/c37a574379ef71e339361954be96be12

[^1]: Some models (3A+, A, A+) will use the "b" equivalents (3B+, B, B+), respectively: https://github.com/raspberrypi/documentation/blob/develop/documentation/asciidoc/computers/configuration/device-tree.adoc#dtbs-overlays-and-configtxt
