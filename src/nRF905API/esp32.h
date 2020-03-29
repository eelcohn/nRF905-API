#ifndef __NRF905_ESP32_H__
#define __NRF905_ESP32_H__

#ifdef ARDUINO_ARCH_ESP32

#include <SPI.h>	// SPIClass

#if NRF905API_HTTPS == 1
#include <WebServerSecure.h>
extern WebServerSecure server;
#else
#include <WebServer.h>
extern WebServer server;
#endif
#if NRF905API_MDNS == 1
#include <ESPmDNS.h>
#endif

// ESP32 devboard pin definitions
#ifndef PIN_AM
#define PIN_AM   32	// nRF905 AM pin (Address Match)
#endif
#ifndef PIN_CD
#define PIN_CD   33	// nRF905 CD pin (Carrier detect)
#endif
#ifndef PIN_CE
#define PIN_CE   27	// nRF905 CS pin (Enable pin)
#endif
#ifndef PIN_DR
#define PIN_DR   35	// nRF905 DR pin (Data Ready)
#endif
#ifndef PIN_PWR
#define PIN_PWR  26	// nRF905 PWR pin (Power mode pin)
#endif
#ifndef PIN_TXEN
#define PIN_TXEN 25	// nRF905 TX_EN pin (Transmit enable pin)
#endif
#ifndef PIN_MOSI
#define PIN_MOSI 13	// nRF905 SPI MOSI pin
#endif
#ifndef PIN_MISO
#define PIN_MISO 12	// nRF905 SPI MISO pin
#endif
#ifndef PIN_SPICLK
#define PIN_SPICLK 14	// nRF905 SPI clock pin
#endif
#ifndef PIN_SPICS
#define PIN_SPICS 15	// nRF905 SPI CS pin
#endif

class Board {
	public:
		String		get_arch(void);
		uint16_t	get_vcc(void);
		uint32_t	get_cpu_id(void);
		uint32_t	get_flash_chip_id(void);
		uint32_t	get_flash_chip_real_size(void);
		String		get_core_version(void);
		uint32_t	get_core_revision(void);
		String		get_hostname(void);
		void		set_hostname(const char *hostname);
		String		get_localIPv6(void);
		bool		enable_IPv6(void);
		void		setPinMode(const uint8_t pin, const uint8_t mode);
		void		writePin(const uint8_t pin, const uint8_t value);
		uint8_t	readPin(const uint8_t pin);
		void		onBoardLED(const bool value);
		bool		SPIBegin(const uint8_t mosi, const uint8_t miso, const uint8_t clk, const uint8_t cs);
		void		SPIEnd(void);
		void		SPISetChipSelectPolarity (const uint8_t cs, const uint8_t active);
		void		SPISetBitOrder(const uint8_t order);
		void		SPISetDataMode(const uint8_t mode);
		void		SPISetFrequency(const uint32_t frequency);
		uint8_t	SPITransfer(const uint8_t out);
		void		SPITransfern(uint8_t * buffer, const size_t size);

	private:
		uint32_t	_spi_frequency;
		uint8_t	_spi_bitorder;
		uint8_t	_spi_datamode;
		uint8_t	_spi_cs;
		uint8_t	_spi_cs_active;
		SPIClass *	spi = NULL;
};

#endif

#endif

