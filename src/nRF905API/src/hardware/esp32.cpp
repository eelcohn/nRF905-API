#ifdef ARDUINO_ARCH_ESP32

#include <EEPROM.h>
#include <SPI.h>
#include <rom/rtc.h>		// RESET_REASON
#include "esp32.h"
#include "../../board.h"	// board-> functions
#include "../../nvram.h"	// NVRAMBuffer
#include "../../utils.h"	// serialPrintHex()

// Create an instance of the server
#if NRF905API_HTTPS == 1
WebServerSecure server(DEFAULT_HTTPS_PORT);
#else
WebServer server(DEFAULT_HTTP_PORT);
#endif



/* ESP32 Class functions */
Board::Board(void) {
	pinMode(PIN_LED, OUTPUT);
}

Board::~Board(void) {
}

const String Board::get_arch(void) {
	return "esp32";
}

void Board::reset(void) {
	ESP.restart();
}

const String Board::restartReason(void) {
	size_t		i;
	String		result = "{\"";

	for (i = 0; i < NUMBER_OF_CPU_CORES; i++) {
		result += String("cpu" + String(i) + "\":\"");
		switch (rtc_get_reset_reason(i)) {
			case 1 :
				result += String("POWERON_RESET");
				break;          /**<1,  Vbat power on reset*/
			case 3 :
				result += String("SW_RESET");
				break;               /**<3,  Software reset digital core*/
			case 4 :
				result += String("OWDT_RESET");
				break;             /**<4,  Legacy watch dog reset digital core*/
			case 5 :
				result += String("DEEPSLEEP_RESET");
				break;        /**<5,  Deep Sleep reset digital core*/
			case 6 :
				result += String("SDIO_RESET");
				break;             /**<6,  Reset by SLC module, reset digital core*/
			case 7 :
				result += String("TG0WDT_SYS_RESET");
				break;       /**<7,  Timer Group0 Watch dog reset digital core*/
			case 8 :
				result += String("TG1WDT_SYS_RESET");
				break;       /**<8,  Timer Group1 Watch dog reset digital core*/
			case 9 :
				result += String("RTCWDT_SYS_RESET");
				break;       /**<9,  RTC Watch dog Reset digital core*/
			case 10 :
				result += String("INTRUSION_RESET");
				break;       /**<10, Instrusion tested to reset CPU*/
			case 11 :
				result += String("TGWDT_CPU_RESET");
				break;       /**<11, Time Group reset CPU*/
			case 12 :
				result += String("SW_CPU_RESET");
				break;          /**<12, Software reset CPU*/
			case 13 :
				result += String("RTCWDT_CPU_RESET");
				break;      /**<13, RTC Watch dog Reset CPU*/
			case 14 :
				result += String("EXT_CPU_RESET");
				break;         /**<14, for APP CPU, reseted by PRO CPU*/
			case 15 :
				result += String("RTCWDT_BROWN_OUT_RESET");
				break;/**<15, Reset when the vdd voltage is not stable*/
			case 16 :
				result += String("RTCWDT_RTC_RESET");
				break;      /**<16, RTC Watch dog reset digital core and rtc module*/
			default :
				result += String("NO_MEAN");
				break;
		}
		if (i < (NUMBER_OF_CPU_CORES - 1))
			result += String("\",\"");
		else
			result += String("\"}");
	}

	return result;
/*
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
*/
}

const uint32_t Board::getCPUFreqMhz(void) {
	return (ESP.getCpuFreqMHz() * 1000000);
}

const char * Board::getSdkVersion(void) {
	return ESP.getSdkVersion();
}

void Board::setADCtoVccMode(void) {
}

const uint16_t Board::get_vcc(void) {
//	return rom_phy_get_vdd33();
	return 0;
}

const uint32_t Board::get_cpu_id(void) {
	return (ESP.getEfuseMac());
}

const uint32_t Board::get_flash_chip_id(void) {
	return 0;
//	return ESP.getFlashChipId();
}

const char * Board::getFlashMode(void) {
	FlashMode_t mode [[gnu::unused]] = ESP.getFlashChipMode();

	return (mode == FM_QIO ? "QIO" : mode == FM_QOUT ? "QOUT" : mode == FM_DIO ? "DIO" : mode == FM_DOUT ? "DOUT" : "UNKNOWN");
}

const uint32_t Board::getFlashChipSpeed(void) {
	return ESP.getFlashChipSpeed();
}

const bool Board::checkFlashCRC(void) {
//	return ESP.checkFlashCRC();
	return true;
}

const uint32_t Board::get_flash_chip_real_size(void) {
//	return ESP.getFlashChipRealSize();
	return 0;
}

const uint32_t Board::getFlashChipSdkSize(void) {
	return ESP.getFlashChipSize();
}

const uint32_t Board::getSketchSize(void) {
	return ESP.getSketchSize();
}

const uint32_t Board::getFreeSketchSpace(void) {
	return ESP.getFreeSketchSpace();
}

const String Board::getSketchMD5(void) {
	return ESP.getSketchMD5();
}

const uint32_t Board::getFreeHeap(void) {
	return ESP.getFreeHeap();
}

const uint32_t Board::getHeapFragmentation(void) {
//	return ESP.getHeapFragmentation();
	return 0;
}

const uint32_t Board::getHeapMaxFreeBlockSize(void) {
//	return ESP.getMaxFreeBlockSize();
	return 0;
}

const String Board::get_core_version(void) {
	return "unknown";
}

const uint32_t Board::get_core_revision(void) {
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

bool Board::getOnBoardLED(void) {
	return _led;
}

void Board::setOnBoardLED(const bool value) {
	this->_led = value;

	if (value == true)
		digitalWrite(PIN_LED, HIGH);
	else
		digitalWrite(PIN_LED, LOW);
}

bool Board::SPIBegin(const uint8_t mosi, const uint8_t miso, const uint8_t clk, const uint8_t cs) {
	this->_spi_cs = cs;
	this->_spi_cs_active = LOW;
	this->_spi = new SPIClass(HSPI);	// TODO: use pin definitions
	this->_spi->begin();

	return true;
}

void Board::SPIEnd(void) {
	_spi->end();
	free(_spi);
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
	_spi->setBitOrder(this->_spi_bitorder);

}

void Board::SPISetDataMode(const uint8_t mode) {
	this->_spi_datamode = mode;
	_spi->setDataMode(this->_spi_datamode);
}

void Board::SPISetFrequency(const uint32_t frequency) {
	this->_spi_frequency = frequency;
	_spi->setFrequency(this->_spi_frequency);
}

uint8_t Board::SPITransfer(const uint8_t out) {
	uint8_t in;

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, LOW);
	else
		digitalWrite(this->_spi_cs, HIGH);

	Serial.print("SPI Tx: ");
	serialPrintHex(&out, 1);

	_spi->beginTransaction(SPISettings(this->_spi_frequency, this->_spi_bitorder, this->_spi_datamode));
	in = _spi->transfer(out);
	_spi->endTransaction();

	Serial.print("SPI Rx: ");
	serialPrintHex(&in, 1);

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, HIGH);
	else
		digitalWrite(this->_spi_cs, LOW);

	return in;
}

void Board::SPITransfern(uint8_t * buffer, const size_t size) {
//	size_t i;

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, LOW);
	else
		digitalWrite(this->_spi_cs, HIGH);

//	Serial.print("SPI Tx len=");
//	Serial.print(size);
//	Serial.print(": ");
//	serialPrintHex(buffer, size);

	_spi->beginTransaction(SPISettings(this->_spi_frequency, this->_spi_bitorder, this->_spi_datamode));
//	for (i = 0; i < size; i++)
//		buffer[i] = _spi->transfer(buffer[i]);
	_spi->transfer(buffer, size);
	_spi->endTransaction();

//	Serial.print("SPI Rx len=");
//	Serial.print(size);
//	Serial.print(": ");
//	serialPrintHex(buffer, size);

	if (this->_spi_cs_active == LOW)
		digitalWrite(this->_spi_cs, HIGH);
	else
		digitalWrite(this->_spi_cs, LOW);
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

void Board::ClearNVRAM(void) {
	size_t i;

	for (i = 0 ; i < EEPROM.length() ; i++)
		EEPROM.write(i, 0xFF);
}

void Board::CommitNVRAM(void) {
	EEPROM.commit();
}

void Board::deepSleep(uint64_t time_us) {
	ESP.deepSleep(time_us);
}

void Board::attachInterrupt(uint8_t pin, void (*handler)(void), int mode) {
	attachInterrupt(pin, handler, mode);
	if (pin == PIN_CD)
		this->_cd_isr_handler_attached = true;
	if (pin == PIN_DR)
		this->_dr_isr_handler_attached = true;
}

void Board::detachInterrupt(uint8_t pin) {
	if ((pin == PIN_CD) && (this->_cd_isr_handler_attached == true)) {
		detachInterrupt(pin);
		this->_cd_isr_handler_attached = false;
	}
	if ((pin == PIN_DR) && (this->_dr_isr_handler_attached == true)) {
		detachInterrupt(pin);
		this->_dr_isr_handler_attached = false;
	}
}

#endif

