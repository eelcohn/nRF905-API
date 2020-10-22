#ifndef __ESP8266_H__
#define __ESP8266_H__

#ifdef ARDUINO_ARCH_ESP8266

#include <SPI.h>

#include <ESP8266WiFi.h>
#if NRF905API_HTTPS == 1
#include <ESP8266WebServerSecure.h>
extern ESP8266WebServerSecure server;
#else
#include <ESP8266WebServer.h>
extern ESP8266WebServer server;
#endif
#if NRF905API_MDNS == 1
#include <ESP8266mDNS.h>
#endif
#if NRF905API_SSDP == 1
#include <ESP8266SSDP.h>
#endif
#include "../../nvram.h"	// NVRAMBuffer

// NodeMCU pin definitions
#ifndef PIN_LED	// On-board LED
#define PIN_LED  2
#endif
#ifndef PIN_AM
#define PIN_AM   -1	// nRF905 AM pin (Address Match) : value is -1 (not connected)
#endif
#ifndef PIN_CD
#define PIN_CD   -1	// nRF905 CD pin (Carrier detect) : value is -1 (not connected)
#endif
#ifndef PIN_CE
#define PIN_CE   D2	// nRF905 CS pin (Enable pin)
#endif
#ifndef PIN_DR
#define PIN_DR   D1	// nRF905 DR pin (Data Ready)
#endif
#ifndef PIN_PWR
#define PIN_PWR  D3	// nRF905 PWR pin (Power mode pin)
#endif
#ifndef PIN_TXEN
#define PIN_TXEN D0	// nRF905 TX_EN pin (Transmit enable pin)
#endif
#ifndef PIN_MOSI
#define PIN_MOSI D7	// nRF905 SPI MOSI pin (GPIO13)
#endif
#ifndef PIN_MISO
#define PIN_MISO D6	// nRF905 SPI MISO pin (GPIO12)
#endif
#ifndef PIN_SPICLK
#define PIN_SPICLK D5	// nRF905 SPI clock pin GPIO14)
#endif
#ifndef PIN_SPICS
#define PIN_SPICS D8	// nRF905 SPI CS pin (GPIO15)
#endif

class Board {
	public:
				Board(void);
				~Board(void);
		const String	get_arch(void);
		void		reset(void);
		const String	restartReason(void);
		const char *	getDateTime(void);
		const uint32_t	getCPUFreqMhz(void);
		const char *	getSdkVersion(void);
		void		setADCtoVccMode(void);
		const uint16_t	get_vcc(void);
		const uint32_t	get_cpu_id(void);
		const uint32_t	get_flash_chip_id(void);
		const uint32_t	getFlashChipSpeed(void);
		const bool	checkFlashCRC(void);
		const char *	getFlashMode(void);
		const uint32_t	get_flash_chip_real_size(void);
		const uint32_t	getFlashChipSdkSize(void);
		const uint32_t	getSketchSize(void);
		const uint32_t	getFreeSketchSpace(void);
		const String	getSketchMD5(void);
		const uint32_t	getFreeHeap(void);
		const uint32_t	getHeapFragmentation(void);
		const uint32_t	getHeapMaxFreeBlockSize(void);
		const String	get_core_version(void);
		const uint32_t	get_core_revision(void);
		String		get_hostname(void);
		void		set_hostname(const char *hostname);
		String		get_localIPv6(void);
		bool		enable_IPv6(void);
		void		setPinMode(const uint8_t pin, const uint8_t mode);
		void		writePin(const uint8_t pin, const uint8_t value);
		uint8_t	readPin(const uint8_t pin);
		bool		getOnBoardLED(void);
		void		setOnBoardLED(const bool value);
		bool		SPIBegin(const uint8_t mosi, const uint8_t miso, const uint8_t clk, const uint8_t cs);
		void		SPIEnd(void);
		void		SPISetChipSelectPolarity (const uint8_t cs, const uint8_t active);
		void		SPISetBitOrder(const uint8_t order);
		void		SPISetDataMode(const uint8_t mode);
		void		SPISetFrequency(const uint32_t frequency);
		uint8_t	SPITransfer(const uint8_t out);
		void		SPITransfern(uint8_t * buffer, const size_t size);
		void		ReadNVRAM(uint8_t * buffer, const size_t offset, const size_t size);
		void		WriteNVRAM(const uint8_t * buffer, const size_t offset, const size_t size);
		void		ClearNVRAM(void);
		void		CommitNVRAM(void);
		void		deepSleep(uint64_t time_us);
		void		attachInterrupt(uint8_t pin, void (*handler)(void), int mode);
		void		detachInterrupt(uint8_t pin);

	private:
		uint32_t	_spi_frequency;
		uint8_t	_spi_bitorder;
		uint8_t	_spi_datamode;
		uint8_t	_spi_cs;
		uint8_t	_spi_cs_active;
		SPIClass *	_spi = NULL;
		bool		_led;
		bool		_cd_isr_handler_attached;
		bool		_dr_isr_handler_attached;
};

#endif

#endif
