# nRF905 Arduino API
nRF905 Arduino API. Compatible with ESP8266 and ESP32 boards, tested on *NodeMCU 1.0 (ESP-12E module)* and *DOIT ESP32 Dev Module*.

## Compiling / Installing
1. Open the .ino file in Arduino IDE
2. Open `config.h` and edit the following entries:
    * `WIFI_SSID`: Enter the name of your WiFi network
    * `WIFI_PASSWORD`: Enter the password of your WiFi network
3. Go to Sketch and select Upload

## Connecting the nRF905 module to your ESP8266 or ESP32 module

#### ESP8266 NodeMCU
| nRF905 pin | ESP8266 pin | ESP8266 GPIO |
|:----------:|:-----------:|:------------:|
|     AM     |      S3     |    GPIO 10   |
|     CD     |      S2     |    GPIO 9    |
|     CE     |      D2     |    GPIO 4    |
|     DR     |      D1     |    GPIO 5    |
|     PWR    |      D3     |    GPIO 0    |
|    TX_EN   |      D0     |    GPIO 16   |
|    MOSI    | D7 / HMOSI  |    GPIO 13   |
|    MISO    | D6 / HMISO  |    GPIO 12   |
|     CLK    | D5 / HCLK   |    GPIO 14   |
|     CS     | D8 / HCS    |    GPIO 15   |

#### ESP32
| nRF905 pin | ESP32 pin | ESP32 GPIO |
|:----------:|:---------:|:----------:|
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

## Usage

### /api/v1/config/
Configure the nRF905.

Example:<br>
`http://192.168.x.y/api/v1/config/?frequency=868400000&crc=16&txpower=10&rxpower=normal&rxaddr=a55a5aa5&txaddr=a55a5aa5&rxaddrwidth=4&txaddrwidth=4&rxpayloadwidth=16&txpayloadwidth=16`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|  `frequency`   |    No    | Set the RF frequency |
|     `crc`      |    No    | `off`: disable CRC<br>`8`: enable 8-bit CRC<br>`16`: enable 16-bt CRC |
|   `txpower`    |    No    | Set the RF transmit power<br>`-10`: -10dBm<br>`-2`: -2dBm<br>`6`: 6dBm<br>`10`: 10dBm |
|   `rxpower`    |    No    | Set the RF receive power<br>`normal`: normal receive mode<br>`low`: low power receive mode |
| `txaddrwidth`  |    No    | Set the number of Tx address bytes<br>`1`: 1 byte<br>`4`: 4 bytes |
| `rxaddrwidth`  |    No    | Set the number of Rx address bytes<br>`1`: 1 byte<br>`4`: 4 bytes |
|`txpayloadwidth`|    No    | Set the number of Tx payload bytes<br>`1...32`: Payload is 1...32 bytes |
|`rxpayloadwidth`|    No    | Set the number of Rx payload bytes<br>`1...32`: Payload is 1...32 bytes |
|    `txaddr`    |    No    | Set the Tx address<br>`0x12345678`: Set Tx address to 0x12345678 |
|    `rxaddr`    |    No    | Set the Rx address<br>`0x12345678`: Set Rx address to 0x12345678 |

### /api/v1/receive/
Not implemented, will probably be removed.

### /api/v1/status/
This will show a status page with details about the hardware. This API call takes no parameters.

Example:<br>
`https://192.168.x.y/api/v1/status/`

### /api/v1/send/
This will transmit data. After the data has been transmitted, the API will wait for data to be received (a reply to the transmitted data). By default it will wait for 2 seconds, but you can change this by adding the `timeout` parameter.

Example:<br>
`https://192.168.x.y/api/v1/send/?addr=aabbccdd&payload=0400035efa0c04a55a5aa50000000000&timeout=10`

| Parameter | Required | Description |
|:---------:|:--------:|:------------|
| `txaddr`  |   Yes    | Address to send the data to |
| `payload` |   Yes    | The data to be sent |
| `timeout` |    No    | Timeout for reading data after the transmission is completed (in seconds)<br>Default: 2 seconds |
