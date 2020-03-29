#ifdef ARDUINO_ARCH_ESP32

#include <SPI.h>
#include "board.h"	// board-> functions
#include "esp32.h"
#include "utils.h"	// serialPrintHex()

// Create an instance of the server
#if NRF905API_HTTPS == 1
WebServerSecure server(NRF905API_WWW_PORT);
#else
WebServer server(NRF905API_WWW_PORT);
#endif



/* ESP32 Class functions */
String Board::get_arch(void) {
	return "esp32";
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
	pinMode(pin, mode);
}

void Board::writePin(const uint8_t pin, const uint8_t value) {
	digitalWrite(pin, value);
}

uint8_t Board::readPin(const uint8_t pin) {
	return digitalRead(pin);
}

void Board::onBoardLED(const bool value) {
	if (value == true)
		digitalWrite(2, LOW);
	else
		digitalWrite(2, HIGH);
}

bool Board::SPIBegin(const uint8_t mosi, const uint8_t miso, const uint8_t clk, const uint8_t cs) {
	this->_spi_cs = cs;
	this->_spi_cs_active = LOW;
	this->spi = new SPIClass(HSPI);	// TODO: use pin definitions
	this->spi->begin();

	return true;
}

void Board::SPIEnd(void) {
	spi->end();
	free(spi);
}

void Board::SPISetChipSelectPolarity (const uint8_t cs, const uint8_t active) {
	this->_spi_cs_active = active;
	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, HIGH);
	else
		digitalWrite(this->_spi_cs, LOW);
}

void Board::SPISetBitOrder(const uint8_t order) {
	this->_spi_bitorder = SPI_MSBFIRST;		// TODO: use order and not fixed value
	spi->setBitOrder(this->_spi_bitorder);

}

void Board::SPISetDataMode(const uint8_t mode) {
	this->_spi_datamode = mode;
	spi->setDataMode(this->_spi_datamode);
}

void Board::SPISetFrequency(const uint32_t frequency) {
	this->_spi_frequency = frequency;
	spi->setFrequency(this->_spi_frequency);
}

uint8_t Board::SPITransfer(const uint8_t out) {
	uint8_t in;

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, LOW);
	else
		digitalWrite(this->_spi_cs, HIGH);

	Serial.print("SPI Tx: ");
	serialPrintHex(&out, 1);

	spi->beginTransaction(SPISettings(this->_spi_frequency, this->_spi_bitorder, this->_spi_datamode));
	in = spi->transfer(out);
	spi->endTransaction();

	Serial.print("SPI Rx: ");
	serialPrintHex(&in, 1);

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, HIGH);
	else
		digitalWrite(this->_spi_cs, LOW);

	return in;
}

void Board::SPITransfern(uint8_t * buffer, const size_t size) {
	size_t i;

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, LOW);
	else
		digitalWrite(this->_spi_cs, HIGH);

	Serial.print("SPI Tx len=");
	Serial.print(size);
	Serial.print(": ");
	serialPrintHex(buffer, size);

	spi->beginTransaction(SPISettings(this->_spi_frequency, this->_spi_bitorder, this->_spi_datamode));
	for (i = 0; i < size; i++)
		buffer[i] = spi->transfer(buffer[i]);
//	spi->transfer(buffer, size);
	spi->endTransaction();

	Serial.print("SPI Rx len=");
	Serial.print(size);
	Serial.print(": ");
	serialPrintHex(buffer, size);

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, HIGH);
	else
		digitalWrite(this->_spi_cs, LOW);
}

#endif

