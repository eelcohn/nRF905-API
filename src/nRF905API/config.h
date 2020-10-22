#ifndef __CONFIG_H__
#define __CONFIG_H__

/* WiFi network settings */
#define WIFI_AP_SSID			"nRF905"
#define WIFI_AP_PASSWORD		"nrf905api"

/* Serial USB port settings */
#define SERIAL_SPEED			115200					// Speed of the serial USB port

/* Include or exclude additional functionality when compiling */
#define NRF905API_HTTPS		0					// Enable HTTPS server
#define NRF905API_IPV6			1					// Enable IPv6
#define NRF905API_MDNS			0					// Enable mDNS
#define NRF905API_NTP			0					// Enable NTP client
#define NRF905API_SSDP			0					// Enable SSDP (Simple Service Discovery Protocol)
#define NRF905API_OTA			0					// Enable OTA (Over-The-Air) firmware upgrades for the ESP module
#define NRF905API_FAN			1					// Include API endpoints for Zehnder & BUVA fan equipment TODO
#define NRF905API_FAN_API		1					// Include API endpoints for Zehnder & BUVA fan equipment TODO
#define NRF905API_WEB_HTML		1					// Include HTMLendpoints TODO

/* Language */
#define LANGUAGE			"en"					// Language

/* nRF905 module configuration */
#define NRF905_XTAL_FREQUENCY		16000000				// nRF905 module XTAL frequency
#define NRF905_CLKOUT_FREQUENCY	500000					// nRF905 clock out frequency (not used)
#define NRF905_SPI_FREQUENCY		10000000				// SPI clock frequency: 10MHz

/* Uncomment to use custom pin definitions */
//#define PIN_AM			10					// nRF905 AM pin (Address Match)
//#define PIN_CD			9					// nRF905 CD pin (Carrier detect)
//#define PIN_CE			D2					// nRF905 CS pin (Enable pin)
//#define PIN_DR			D1					// nRF905 DR pin (Data Ready)
//#define PIN_PWR			D3					// nRF905 PWR pin (Power mode pin)
//#define PIN_TXEN			D0					// nRF905 TX_EN pin (Transmit enable pin)
//#define SPI_MOSI			13					// nRF905 CSN pin (SPI slave select pin)
//#define SPI_MISO			12					// nRF905 CSN pin (SPI slave select pin)
//#define SPI_CLK			14					// nRF905 CSN pin (SPI slave select pin)
//#define SPI_CS			15					// nRF905 CSN pin (SPI slave select pin)

#endif

