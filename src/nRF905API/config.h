#ifndef __NRF905API_CONFIG_H__
#define __NRF905API_CONFIG_H__

/* WiFi network settings */
#define WIFI_HOSTNAME		"nrf905-api"
#define WIFI_SSID		"WiFiNetwork"
#define WIFI_PASSWORD		"SuperSecretPassword"

/* Serial USB port settings */
#define SERIAL_SPEED		115200		// Speed of the serial USB port

/* Include or exclude additional functionality */
#define NRF905API_HTTPS	0		// Enable HTTPS server
#define NRF905API_IPV6		0		// Enable IPv6
#define NRF905API_MDNS		0		// Enable mDNS
#define NRF905API_NTP		0		// Enable NTP client
#define NRF905API_SSDP		0		// Enable SSDP (Simple Service Discovery Protocol)
#define NRF905API_OTA		0		// Enable OTA (Over-The-Air) firmware upgrades for the ESP module
#define NRF905API_ZEHNDER	1		// Include API endpoints for Zehnder equipment

/* Advanced settings / Settings for additional functionality */
#define NRF905API_NTP_SERVER	"pool.ntp.org"
#define NRF905API_OTA_PORT	8233		// OTA TCP port
#define NRF905API_OTA_PASSWORD	"c19555f1193996117d477eebde5c913e" // MD5 password hash for password `nrf905`
#define NRF905API_WWW_USERNAME	"admin"  	// HTTP authentication: username
#define NRF905API_WWW_PASSWORD	"nrf905"	// HTTP authentication: password TODO use hash value (if possible)
#define NRF905API_WWW_PORT	80
#define NRF905API_SPI_FREQUENCY 10000000	// SPI clock frequency: 10MHz

/* Uncomment to use custom pin definitions */
//#define PIN_AM   10	// nRF905 AM pin (Address Match)
//#define PIN_CD   9	// nRF905 CD pin (Carrier detect)
//#define PIN_CE   D2	// nRF905 CS pin (Enable pin)
//#define PIN_DR   D1	// nRF905 DR pin (Data Ready)
//#define PIN_PWR  D3	// nRF905 PWR pin (Power mode pin)
//#define PIN_TXEN D0	// nRF905 TX_EN pin (Transmit enable pin)
//#define SPI_MOSI 13	// nRF905 CSN pin (SPI slave select pin)
//#define SPI_MISO 12	// nRF905 CSN pin (SPI slave select pin)
//#define SPI_CLK  14	// nRF905 CSN pin (SPI slave select pin)
//#define SPI_CS   15	// nRF905 CSN pin (SPI slave select pin)

#endif

