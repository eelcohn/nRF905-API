v1.0.0
* The device will create a WiFi access point when no WiFi is configured, or when it can't connect to the configured WiFi network
* Configuration can now be now stored in NVRAM/EEPROM
* Improved reliability: Transmitting and receiving data is now handled by ISR's (Interrupt Service Routines)
* Improved reliability: All transmissions are now retransmitted 4 times
* On-board LED is now used to reflect status of Carrier Detect (ESP32 boards) or Data Ready (ESP8266 boards)
* Added HTML configuration wizard pages
* Added HTML fan configuration page
* Added HTML fan control page
* Added preliminary multi-language support
* Added preliminary support for Raspberry Pi boards
* Added endpoint /api/v2/fan/config.json
* Added endpoint /api/v2/fan/link.json
* Added endpoint /api/v2/fan/setvoltage.json
* Added endpoint /api/v2/fan/setspeed.json
* Added endpoint /api/v2/fan/settimer.json
* Deprecated endpoint /api/v1/zehnder/config.json
* Deprecated endpoint /api/v1/zehnder/link.json
* Deprecated endpoint /api/v1/zehnder/setpower.json
* Deprecated endpoint /api/v1/zehnder/settimer.json
* Added nvram=clear parameter to endpoint /api/v1/systemconfig.json
* Changed message when no IPv6 address is configured from 'failed' to 'n/a'
* Fixed NTP client
* Fixed power reset reason on ESP32 CPU's
* Removed ArduinoJson depencency
* Show user friendly error when compiling on non-supported platform/board

v0.3.2
* Fixed autoconfiguration for endpoint /api/v1/zehnder/config.json
* Fixed autoconfiguration for endpoint /api/v1/zehnder/link.json

v0.3.1
* Added endpoint /api/v1/zehnder/config.json
* Added endpoint /api/v1/zehnder/link.json
* Added endpoint /api/v1/zehnder/setpower.json
* Added endpoint /api/v1/zehnder/settimer.json
* Fixed stability issues

v0.3.0
* Renamed endpoints to .json
* Added /api/v1/systemconfig.json endpoint
* Set nRF905 to idle mode when writing config file
* Updated switching nRF905 modes when transmitting data
* Added storing the nRF905 config settings in NVRAM/EEPROM
* Fixed voltage measurement for ESP8266 boards
* Fixed stability issues

v0.2.1
* First public beta release

v0.2.0
* Unreleased test version

v0.1.1
* Unreleased test version

v0.1.0
* First test version

