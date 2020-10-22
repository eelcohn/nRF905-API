# Building the hardware
For this project you will need the following components:
1. Any ESP8266 or ESP32 based board, like the *Wemos D1 mini Pro*, *NodeMCU 1.0 (ESP-12E module)* or the *DOIT ESP32 Dev Module*.
2. An nRF905 module, for example the PTR8000
3. Fourteen (14) female-to-female wires
4. An 5V micro-USB power supply, e.g. an old micro-USB phone charger

## Connecting the nRF905 module to your ESP8266 or ESP32 module

#### NodeMCU 1.0 ESP12E
![NodeMCU pinout](https://github.com/eelcohn/nRF905-API/blob/master/images/nodemcu_pins.png)
| nRF905 pin | NodeMCU pin | NodeMCU GPIO |
|:----------:|:-----------:|:------------:|
|     Vcc    |     3.3V    |    3.3V      |
|     Gnd    |      Gnd    |     Gnd      |
|     AM     |      -      |      -       |
|     CD     |      -      |      -       |
|     CE     |      D2     |    GPIO 4    |
|     DR     |      D1     |    GPIO 5    |
|     PWR    |      D3     |    GPIO 0    |
|    TX_EN   |      D0     |    GPIO 16   |
|    MOSI    | D7 / HMOSI  |    GPIO 13   |
|    MISO    | D6 / HMISO  |    GPIO 12   |
|     CLK    | D5 / HCLK   |    GPIO 14   |
|     CS     | D8 / HCS    |    GPIO 15   |

The NodeMCU doens't have enough GPIO pins for CD and AM to connect to.

##### Manually flash the firmware to the ESP8266 NodeMCU :
`esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash 0x0 nRF905API.ino.bin`

#### Wemos D1 mini / Wemos D1 mini pro
![Wemos D1 mini pinout](https://github.com/eelcohn/nRF905-API/blob/master/images/Wemos-D1-mini.png)
| nRF905 pin | Wemos D1 pin | Wemos D1 GPIO |
|:----------:|:------------:|:-------------:|
|     Vcc    |     3.3V     |    3.3V       |
|     Gnd    |      Gnd     |     Gnd       |
|     AM     |      -       |      -        |
|     CD     |      -       |      -        |
|     CE     |      D2      |    GPIO 4     |
|     DR     |      D1      |    GPIO 5     |
|     PWR    |      D3      |    GPIO 0     |
|    TX_EN   |      D0      |    GPIO 16    |
|    MOSI    | D7 / HMOSI   |    GPIO 13    |
|    MISO    | D6 / HMISO   |    GPIO 12    |
|     CLK    | D5 / HCLK    |    GPIO 14    |
|     CS     | D8 / HCS     |    GPIO 15    |

The Wemos D1 mini doens't have enough GPIO pins for CD and AM to connect to.

##### Manually flash the firmware on the Wemos D1 mini:
`esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash 0x0 WemosD1.ino.bin`

#### DOIT ESP32 devkit v1
![ESP32 DevKit 30-pin version pinout](https://github.com/eelcohn/nRF905-API/blob/master/images/ESP32-DOIT-DEVKIT-V1-Board-Pinout-30-GPIOs.png)
![ESP32 DevKit 36-pin version pinout](https://github.com/eelcohn/nRF905-API/blob/master/images/ESP32-DOIT-DEVKIT-V1-Board-Pinout-36-GPIOs.jpg)
| nRF905 pin | ESP32 pin | ESP32 GPIO |
|:----------:|:---------:|:----------:|
|     Vcc    |    3.3V   |    3.3V    |
|     Gnd    |    Gnd    |    Gnd     |
|     AM     |    D32    |  GPIO 32   |
|     CD     |    D33    |  GPIO 33   |
|     CE     |    D27    |  GPIO 27   |
|     DR     |    D35    |  GPIO 35   |
|     PWR    |    D26    |  GPIO 26   |
|    TX_EN   |    D25    |  GPIO 25   |
|    MOSI    |    D13    |  GPIO 13   |
|    MISO    |    D12    |  GPIO 12   |
|     CLK    |    D14    |  GPIO 14   |
|     CS     |    D15    |  GPIO 15   |

##### Manually flash the firmware on the DOIT ESP32 devkit v1:
`esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 DOIT-ESP32.ino.bin 0x8000 DOIT-ESP32.ino.partitions.bin`
