# nRF905 Arduino API
nRF905 Arduino API. Compatible with ESP8266 and ESP32 boards, tested on *Wemos D1 mini Pro*, *NodeMCU 1.0 (ESP-12E module)* and *DOIT ESP32 Dev Module*.

## Building the hardware
See [Buiding the hardware](https://github.com/eelcohn/nRF905-API/blob/master/HARDWARE.md)

## Compiling / Installing
1. Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software)
2. Open the [nRF905API.ino](https://github.com/eelcohn/nRF905-API/blob/master/src/nRF905API/nRF905API.ino) file in Arduino IDE
3. In the *File* menu select *Preferences* and add the following to *Additional Board Manager URLs*:
```
https://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json
```
4. In the *Sketch* menu select *Board* and find the board you are using (DO-IT ESP32 / Wemos / NodeMCU etc).
5. In the *Sketch* menu select *Upload*
6. When the code has been uploaded the device will reboot and start a WiFi access point named `nRF905`. Connect to that access point with the password `nrf905api`.
7. Open a webbrowser and go to `http://192.168.4.1/`
8. Follow the configuration wizard:<br>
![cfglang](https://github.com/eelcohn/nRF905-API/blob/master/images/cfglang.png)<br>
![cfgauth](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgauth.png)<br>
![cfgwifi](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgwifi.png)<br>
![cfgfan](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgfan.png)<br>
![cfgfand1](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgfand1.png)<br>
![cfgfand2](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgfand2.png)<br>
![cfgboot](https://github.com/eelcohn/nRF905-API/blob/master/images/cfgboot.png)<br>
9. Your device will now connect to your WiFi network.

## Usage

### /api/v1/config.json
Configure the nRF905.

Example:<br>
`http://192.168.x.y/api/v1/config.json?frequency=868400000&crc=16&txpower=10&rxpower=normal&rxaddr=a55a5aa5&txaddr=a55a5aa5&rxaddrwidth=4&txaddrwidth=4&rxpayloadwidth=16&txpayloadwidth=16`

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
|    `nvram`     |    No    | Store the nRF905 configuration in NVRAM<br>`true`: Store nRF905 config in NVRAM<br>`false`: Do not store nRF905 config in NVRAM |

### /api/v1/status.json
This will show a status page with details about the hardware. This API call takes no parameters.

Example:<br>
`http://192.168.x.y/api/v1/status.json`

### /api/v1/receive.json
This will return a list of received data frames by the nRF905. This API call takes no parameters.

Example:<br>
`http://192.168.x.y/api/v1/receive.json`

### /api/v1/send.json
This will transmit data. After the data has been transmitted, the API will wait for data to be received (a reply to the transmitted data). By default it will wait for 2 seconds, but you can change this by adding the `timeout` parameter.

Example:<br>
`http://192.168.x.y/api/v1/send.json?txaddr=aabbccdd&payload=0400035efa0c04a55a5aa50000000000&timeout=10`

| Parameter | Required | Description |
|:---------:|:--------:|:------------|
| `txaddr`  |   Yes    | Address to send the data to |
| `payload` |   Yes    | The data to be sent |
| `timeout` |    No    | Timeout for reading data after the transmission is completed (in seconds)<br>Default: 2 seconds |

### /api/v1/systemconfig.json
Configure the system board.

Example:<br>
`http://192.168.x.y/api/v1/systemconfig.json?reset=true`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|    `reset `    |    No    | Reset the system<br>`true`: Reset system |
|    `nvram `    |    No    | Clear the NVRAM<br>`clear`: Clear the NVRAM |

## Fan API calls

### /api/v2/fan/link.json

Link the nRF905-API to a fan device. This API call takes no parameters.

Example:<br>
`http://192.168.x.y/api/v2/fan/link.json`

### /api/v2/fan/config.json

Configure the fan parameters.

Example:<br>
`http://192.168.x.y/api/v2/fan/config.json?model=zehnder&network=1a2b3c4d&main_unit_id=91&device_id=5d`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|    `model`     |   Yes    | Fan model:<br>`zehnder`: Zehnder fans (868.400MHz)<br>`buva`: BUVA fans (868.200MHz) |
|   `network`    |   Yes    | Network ID of your fan network |
| `main_unit_id` |   Yes    | ID of your fan main unit |
|  `device_id`   |   Yes    | ID of your nRF905-API |

### /api/v2/fan/setspeed.json

Set the fan speed of your fan device.

Example:<br>
`http://192.168.x.y/api/v2/fan/setspeed.json?speed=high`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|    `speed`     |   Yes    | Fan speed:<br>`low`: Low speed<br>`medium`: Medium speed<br>`high`: High speed<br>`max`: Maximum speed |
|    `timer`     |   No     | Set the fan timer (number of minutes) |

### /api/v2/fan/setvoltage.json

Set the fan voltage of your fan device.

Example:<br>
`http://192.168.x.y/api/v2/fan/setvoltage.json?voltage=2.4`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|   `voltage`    |   Yes    | Voltage:<br>`0.0`...`10.0`: Voltage; range between 0.0 volt and 10.0 volt |

### /api/test/fan/querydevice.json

Query a device for it's current settings (this API endpoint is not yet finished and is in test-mode).

Example:<br>
`http://192.168.x.y/api/test/fan/querydevice.json?device_id=32`

|  Parameter     | Required | Description |
|:--------------:|:--------:|:------------|
|  `device_id`   |   No     | Device ID:<br>ID of a fan device. By default it will select the fan it's linked to |
