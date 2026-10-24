#ifdef RPI

#include <SPI.h>
#include "../../board.h"	// board-> functions
#include "../../utils.h"	// serialPrintHex()
#include "esp32.h"

// Create an instance of the server
#if NRF905API_HTTPS == 1
WebServerSecure server(DEFAULT_HTTPS_PORT);
#else
WebServer server(DEFAULT_HTTP_PORT);
#endif



/* ESP32 Class functions */
String Board::get_arch(void) {
	return "bcm2835";
}

uint16_t Board::get_vcc(void) {
	return 0;
}

uint32_t Board::get_cpu_id(void) {
	return (ESP.getEfuseMac());
}

uint32_t Board::get_flash_chip_id(void) {
	return 0;
}

uint32_t Board::get_flash_chip_real_size(void) {
	return 0;
}

String Board::get_core_version(void) {
	return "unknown";
}

uint32_t Board::get_core_revision(void) {
	return 0;
}

String Board::get_hostname(void) {
	return WiFi.getHostname();
}

void Board::set_hostname(const char *hostname) {
	WiFi.setHostname(hostname);
}

String Board::get_localIPv6(void) {
	return WiFi.localIPv6().toString();
}

bool Board::enable_IPv6(void) {
	return WiFi.enableIpV6();
}

void Board::setPinMode(const uint8_t pin, const uint8_t mode) {
	bcm2835_gpio_write(pin, mode);
}

void Board::writePin(const uint8_t pin, const uint8_t value) {
	bcm2835_gpio_write(pin, value);
}

uint8_t Board::readPin(const uint8_t pin) {
	return bcm2835_gpio_lev(pin);
}

bool Board::getOnBoardLED(void) {
	return led;
}

void Board::setOnBoardLED(const bool value) {
//	if (value == true)
//		digitalWrite(2, LOW);
//	else
//		digitalWrite(2, HIGH);
}

bool Board::SPIBegin(const uint8_t mosi, const uint8_t miso, const uint8_t clk, const uint8_t cs) {
	this->_spi_cs = cs;
	this->_spi_cs_active = LOW;
	bcm2835_spi_begin();
	bcm2835_spi_chipSelect(this->_spi_cs);

	return true;
}

void Board::SPIEnd(void) {
	spi->end();
	free(spi);
}

void Board::SPISetChipSelectPolarity (const uint8_t cs, const uint8_t active) {
	this->_spi_cs_active = active;
	bcm2835_spi_setChipSelectPolarity(nrf->spi_cs, active);
}

void Board::SPISetBitOrder(const uint8_t order) {
	this->_spi_bitorder = BCM2835_SPI_BIT_ORDER_MSBFIRST;		// TODO: use order and not fixed value
	bcm2835_spi_setBitOrder(this->_spi_bitorder);

}

void Board::SPISetDataMode(const uint8_t mode) {
	this->_spi_datamode = BCM2835_SPI_MODE0;
	bcm2835_spi_setDataMode(this->_spi_datamode);
}

void Board::SPISetFrequency(const uint32_t frequency) {
	this->_spi_frequency = frequency;
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
}

uint8_t Board::SPITransfer(const uint8_t out) {
	uint8_t in;

//	Serial.print("SPI Tx: ");
//	serialPrintHex(&out, 1);

	spi->beginTransaction(SPISettings(this->_spi_frequency, this->_spi_bitorder, this->_spi_datamode));
	in = spi->transfer(out);
	spi->endTransaction();

//	Serial.print("SPI Rx: ");
//	serialPrintHex(&in, 1);

	return in;
}

void Board::ReadNVRAM(uint8_t * buffer, const size_t offset, const size_t size) {
	size_t i;

	for (i = 0; i < size; i++)
		buffer[i] = EEPROM.read(offset + i);
}

void Board::WriteNVRAM(const uint8_t * buffer, const size_t offset, const size_t size) {
	size_t i;

	for (i = 0; i < size; i++)
		EEPROM.write((offset + i), buffer[i]);
}

void Board::SPITransfern(uint8_t * buffer, const size_t size) {
	size_t i;

//	Serial.print("SPI Tx len=");
//	Serial.print(size);
//	Serial.print(": ");
//	serialPrintHex(buffer, size);

	bcm2835_spi_transfern(buffer, size);
//	spi->transfer(buffer, size);

//	Serial.print("SPI Rx len=");
//	Serial.print(size);
//	Serial.print(": ");
//	serialPrintHex(buffer, size);
}

#endif

