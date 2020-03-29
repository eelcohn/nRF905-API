/*
  nRF905 Class
  written by Eelco Huininga 2020
*/

#include <cstdint>	// uint8_t, uint16_t, uint32_t
#include "nRF905.h"
#include "board.h"	// Board * board

extern Board * board;

nRF905::nRF905(uint8_t am, uint8_t cd, uint8_t ce, uint8_t dr, uint8_t pwr, uint8_t txen, uint8_t spi_cs, uint32_t SPIFrequency) {
	_am = am;
	_cd = cd;
	_ce = ce;
	_spi_cs = spi_cs;
	_dr = dr;
	_pwr = pwr;
	_txen = txen;
	_spi_frequency = SPIFrequency;
}

nRF905::~nRF905(void) {
}

bool nRF905::init(void) {
	board->setPinMode(_am, INPUT);
	board->setPinMode(_cd, INPUT);
	board->setPinMode(_ce, OUTPUT);
	board->setPinMode(_spi_cs, OUTPUT);
	board->setPinMode(_dr, INPUT);
	board->setPinMode(_pwr, OUTPUT);
	board->setPinMode(_txen, OUTPUT);
	board->writePin(_ce, HIGH);
	board->writePin(_spi_cs, LOW);
	board->writePin(_pwr, HIGH);
	board->writePin(_txen, LOW);

	board->SPIBegin(PIN_MISO, PIN_MOSI, PIN_SPICLK, PIN_SPICS);
	board->SPISetDataMode(SPI_MODE0);
	board->SPISetBitOrder(MSBFIRST);
	board->SPISetFrequency(this->_spi_frequency);
	board->writePin(_spi_cs, HIGH);

	/* Get default config from the nRF905 */
	this->readConfigRegisters();
	this->decodeConfigRegisters();

	return true;
}

uint8_t nRF905::getStatus(void) {
	return this->_config_registers.command;
}

uint32_t nRF905::getFrequency(void) {
	return this->_frequency;
}

bool nRF905::setFrequency(const uint32_t frequency) {
	if ((frequency < 422400000) || (frequency > 947000000))
		return false;
	if ((frequency > 473500000) && (frequency < 844800000))
		return false;
	if (frequency > 473500000) {
		this->_band = true;
		this->_channel = (((frequency / 2) - 422400000) / 100000);
	} else {
		this->_band = false;
		this->_channel = ((frequency - 422400000) / 100000);
	}
	this->_frequency = frequency;

	return true;
}

uint8_t nRF905::getTxPower(void) {
	return this->_tx_power;
}

bool nRF905::setTxPower(const int8_t power) {
	switch (power) {
		case -10 :
		case -2  :
		case  6  :
		case  10 :
			this->_tx_power = power;
			break;

		default :
			return false;
			break;
	}

	return true;
}

bool nRF905::getRxReducedPower(void) {
	return this->_rx_power;
}

void nRF905::setRxReducedPower(const bool power) {
	this->_rx_power = power;
}

bool nRF905::getAutoRetransmit(void) {
	return this->_auto_retransmit;
}

void nRF905::setAutoRetransmit(const bool retransmit) {
	this->_auto_retransmit = retransmit;
}

uint8_t nRF905::getRxAddressWidth(void) {
	return this->_rx_address_width;
}

bool nRF905::setRxAddressWidth(const int8_t width) {
	switch (width) {
		case 1 :
		case 4 :
			this->_rx_address_width = width;
			break;

		default :
			return false;
			break;
	}

	return true;
}

uint8_t nRF905::getTxAddressWidth(void) {
	return this->_tx_address_width;
}

bool nRF905::setTxAddressWidth(const int8_t width) {
	switch (width) {
		case 1 :
		case 4 :
			this->_tx_address_width = width;
			break;

		default :
			return false;
			break;
	}

	return true;
}

uint8_t nRF905::getRxPayloadWidth(void) {
	return this->_rx_payload_width;
}

bool nRF905::setRxPayloadWidth(const int8_t width) {
	if ((width > 0) && (width < 33)) {
		this->_rx_payload_width = width;
		return true;
	}
	return false;
}

uint8_t nRF905::getTxPayloadWidth(void) {
	return this->_tx_payload_width;
}

bool nRF905::setTxPayloadWidth(const int8_t width) {
	if ((width > 0) && (width < 33)) {
		this->_tx_payload_width = width;
		return true;
	}
	return false;
}

uint32_t nRF905::getRxAddress(void) {
	return this->_rx_address;
}

bool nRF905::setRxAddress(const int32_t address) {
	this->_rx_address = address;
	return true;
}

uint32_t nRF905::getTxAddress(void) {
	return this->_tx_address;
}

bool nRF905::setTxAddress(const int32_t address) {
	this->_tx_address = address;
	return true;
}

bool nRF905::getCRC(void) {
	return this->_crc_enable;
}

void nRF905::setCRC(const bool crc) {
	this->_crc_enable = crc;
}

uint8_t nRF905::getCRCbits(void) {
	return this->_crc_bits;
}

bool nRF905::setCRCbits(const uint8_t crc_bits) {
	switch (crc_bits) {
		case  8 :
		case 16 :
			this->_crc_bits = crc_bits;
			break;

		default :
			return false;
			break;
	}

	return true;
}

uint32_t nRF905::getXtalFrequency(void) {
	return this->_xtal_frequency;
}

bool nRF905::setXtalFrequency(const uint32_t frequency) {
	switch (frequency) {
		case  4000000 :
		case  8000000 :
		case 12000000 :
		case 16000000 :
		case 20000000 :
			this->_xtal_frequency = frequency;
			break;

		default :
			return false;
			break;
	}

	return true;
}

bool nRF905::getClkOut(void) {
	return this->_clk_out_enable;
}

void nRF905::setClkOut(const bool clk_out_enable) {
	this->_clk_out_enable = clk_out_enable;
}

uint32_t nRF905::getClkOutFrequency(void) {
	return this->_clk_out_frequency;
}

bool nRF905::setClkOutFrequency(const uint32_t frequency) {
	switch (frequency) {
		case 4000000 :
		case 2000000 :
		case 1000000 :
		case  500000 :
			this->_clk_out_frequency = frequency;
			break;

		default :
			return false;
			break;
	}

	return true;
}


void nRF905::setModeIdle(void) {
	if (_mode != Idle) {
		board->writePin(_ce, LOW);
		board->writePin(_txen, LOW);
		_mode = Idle;
	}
}

void nRF905::setModeRx(void) {
	if (_mode != Rx) {
		board->writePin(_txen, LOW);
		board->writePin(_ce, HIGH);
		_mode = Rx;
	}
}

void nRF905::setModeTx(void) {
	if (_mode != Tx) {
		board->writePin(_txen, HIGH);
		board->writePin(_ce, HIGH);
		_mode = Tx;
	}
}

void nRF905::decodeConfigRegisters(void) {
	this->_channel			= ((this->_config_registers.config[1] & 0x01) << 8) | this->_config_registers.config[0];
	this->_band			= (this->_config_registers.config[1] & 0x02) ? true : false;
	this->_rx_power		= (this->_config_registers.config[1] & 0x10) ? true : false;
	this->_auto_retransmit		= (this->_config_registers.config[1] & 0x20) ? true : false;
	this->_rx_address_width	= this->_config_registers.config[2] & 0x07;
	this->_tx_address_width	= (this->_config_registers.config[2] >> 4) & 0x07;
	this->_rx_payload_width 	= this->_config_registers.config[3] & 0x3F;
	this->_tx_payload_width 	= this->_config_registers.config[4] & 0x3F;
	this->_rx_address		= ((this->_config_registers.config[8] << 24) | (this->_config_registers.config[7] << 16) | (this->_config_registers.config[6] << 8) | this->_config_registers.config[5]);
	this->_clk_out_frequency	= (4000000 >> (this->_config_registers.config[9] & 0x03));
	this->_clk_out_enable		= (this->_config_registers.config[9] & 0x04) ? true : false;
	this->_xtal_frequency		= (((this->_config_registers.config[9] >> 3) & 0x07) + 1) * 4000000;
	this->_crc_enable		= (this->_config_registers.config[9] & 0x40) ? true : false;
	this->_crc_bits		= (this->_config_registers.config[9] & 0x80) ? 16 : 8;

	this->_frequency = ((422400000 + (this->_channel * 100000)) * (this->_band ? 2 : 1));
	switch ((this->_config_registers.config[1] >> 2) & 0x03) {
		case 0x00 :
			this->_tx_power = -10;
			break;
		case 0x01 :
			this->_tx_power = -2;
			break;
		case 0x02 :
			this->_tx_power =   6;
			break;
		case 0x03 :
			this->_tx_power =  10;
			break;
		default :
			this->_tx_power =  10;
			break;
	}		

}

void nRF905::encodeConfigRegisters(void) {
	uint8_t freq, tx_power;

	switch (this->_clk_out_frequency) {
		case 4000000 :
			freq = 0x00;
			break;
		case 2000000 :
			freq = 0x01;
			break;
		case 1000000 :
			freq = 0x02;
			break;
		case  500000 :
			freq = 0x03;
			break;
		default :
			freq = 0x00;
			break;
	}

	switch (this->_tx_power) {
		case -10 :
			tx_power = 0x00;
			break;
		case -2 :
			tx_power = 0x04;
			break;
		case  6 :
			tx_power = 0x08;
			break;
		case  10 :
			tx_power = 0x0C;
			break;
		default :
			tx_power = 0x0C;
			break;
	}

	this->_config_registers.config[0]  = (this->_channel & 0xFF);
	this->_config_registers.config[1]  = (this->_channel >> 8) & 0x01;
	this->_config_registers.config[1] |= (this->_band ? 0x02 : 0x00);
	this->_config_registers.config[1] |= tx_power;
	this->_config_registers.config[1] |= (this->_rx_power ? 0x10 : 0x00);
	this->_config_registers.config[1] |= (this->_auto_retransmit ? 0x20 : 0x00);
	this->_config_registers.config[2]  = (this->_rx_address_width & 0x07);
	this->_config_registers.config[2] |= (this->_tx_address_width & 0x07) << 4;
	this->_config_registers.config[3]  = (this->_rx_payload_width & 0x3F);
	this->_config_registers.config[4]  = (this->_tx_payload_width & 0x3F);
	this->_config_registers.config[5]  = (this->_rx_address & 0xFF);
	this->_config_registers.config[6]  = (this->_rx_address >> 8) & 0xFF;
	this->_config_registers.config[7]  = (this->_rx_address >> 16) & 0xFF;
	this->_config_registers.config[8]  = (this->_rx_address >> 24) & 0xFF;
	this->_config_registers.config[9]  = freq;
	this->_config_registers.config[9] |= (this->_clk_out_enable ? 0x04 : 0x00);
	this->_config_registers.config[9] |= ((this->_xtal_frequency / 4000000) - 1) << 3;
	this->_config_registers.config[9] |= (this->_crc_enable ? 0x40 : 0x00);
	this->_config_registers.config[9] |= (this->_crc_bits == 8) ? 0x00 : 0x80;
}

void nRF905::readConfigRegisters(void) {
	this->_config_registers.command = NRF905_COMMAND_R_CONFIG;
	memset(this->_config_registers.config, 0, sizeof(this->_config_registers.config));
	board->SPITransfern(this->_config_registers.buffer, sizeof(this->_config_registers.buffer));
	this->_status = this->_config_registers.command;
}

bool nRF905::writeConfigRegisters(void) {
	this->_config_registers.command = NRF905_COMMAND_W_CONFIG;
	board->SPITransfern(this->_config_registers.buffer, sizeof(this->_config_registers.buffer));
	this->_status = this->_config_registers.command;
	return true;
}

void nRF905::readTxPayload(uint8_t * buffer) {
	this->_tx_payload.command = NRF905_COMMAND_R_TX_PAYLOAD;
	memset(this->_tx_payload.payload, 0, sizeof(this->_tx_payload.buffer));
	board->SPITransfern(this->_tx_payload.buffer, sizeof(this->_tx_payload.buffer));
	memcpy(buffer, this->_tx_payload.payload, sizeof(this->_tx_payload.payload));
//	memcpy(buffer, &this->_tx_payload.payload, sizeof(this->_tx_payload.payload));
	this->_status = this->_config_registers.command;
}

void nRF905::writeTxPayload(const uint8_t * buffer) {
	this->_tx_payload.command = NRF905_COMMAND_W_TX_PAYLOAD;
	memcpy(this->_tx_payload.payload, buffer, sizeof(this->_tx_payload.payload));
	board->SPITransfern(this->_tx_payload.buffer, sizeof(this->_tx_payload.buffer));
	this->_status = this->_config_registers.command;
}

void nRF905::readRxPayload(uint8_t * buffer) {
	this->_rx_payload.command = NRF905_COMMAND_R_RX_PAYLOAD;
	memset(this->_rx_payload.payload, 0, sizeof(this->_rx_payload.payload));
	board->SPITransfern(this->_rx_payload.buffer, sizeof(this->_rx_payload.buffer));
	memcpy(buffer, this->_rx_payload.payload, sizeof(this->_rx_payload.payload));
	this->_status = this->_config_registers.command;
}

void nRF905::readTxAddress(void) {
	nRF905AddressBuffer tx_addressbuffer;

	tx_addressbuffer.command = NRF905_COMMAND_R_TX_ADDRESS;
	tx_addressbuffer.address[0] = 0x00;
	tx_addressbuffer.address[1] = 0x00;
	tx_addressbuffer.address[2] = 0x00;
	tx_addressbuffer.address[3] = 0x00;
	board->SPITransfern(tx_addressbuffer.buffer, sizeof(tx_addressbuffer.buffer));
	this->_tx_address  = tx_addressbuffer.address[3];
	this->_tx_address |= (tx_addressbuffer.address[2] <<  8);
	this->_tx_address |= (tx_addressbuffer.address[1] << 16);
	this->_tx_address |= (tx_addressbuffer.address[0] << 24);
	this->_status = tx_addressbuffer.command;
}

void nRF905::writeTxAddress(void) {
	nRF905AddressBuffer tx_addressbuffer;

	tx_addressbuffer.command = NRF905_COMMAND_W_TX_ADDRESS;
	tx_addressbuffer.address[3] = (this->_tx_address >> 24) & 0xFF;
	tx_addressbuffer.address[2] = (this->_tx_address >> 16) & 0xFF;
	tx_addressbuffer.address[1] = (this->_tx_address >>  8) & 0xFF;
	tx_addressbuffer.address[0] = (this->_tx_address      ) & 0xFF;
	board->SPITransfern(tx_addressbuffer.buffer, sizeof(tx_addressbuffer.buffer));
	this->_status = tx_addressbuffer.command;
}

void nRF905::channelConfig(void) {
	uint8_t buffer[2];

	buffer[0] = NRF905_COMMAND_CHANNEL_CONFIG;
	switch (this->_tx_power) {
		case -10 :
			break;

		case -2  :
			buffer[0] |= 0x04;
			break;

		case  6  :
			buffer[0] |= 0x08;
			break;

		case  10 :
			buffer[0] |= 0x0C;
			break;

		default :
			break;
	}
	if (this->_band)
		buffer[0] |= 0x02;
	if (this->_channel & 0x100)
		buffer[0] |= 0x01;
	buffer[1] = this->_channel & 0xFF;

	board->SPITransfern(buffer, sizeof(buffer));
	this->_status = buffer[0];
}

bool nRF905::startTx(uint32_t retransmit, uint32_t timeout) {
	unsigned long startTime;
	uint32_t txdelay;
	bool done;

	// Set or clear retransmit flag
	if (this->getAutoRetransmit() == false) {
		// Set retransmit flag if it wasn't set already
		if (retransmit != 0) {
			this->setAutoRetransmit(true);
			this->encodeConfigRegisters();
			this->writeConfigRegisters();
		}
		txdelay = retransmit;				// TODO replace txdelay with retransmit loop; DR=HIGH at beginning of transmit in retrans-mode, LOW at end of transmit in retrans-mode
	} else {
		// Clear retransmit flag if it wasn't cleared already
		if (retransmit == 0) {
			this->setAutoRetransmit(false);
			this->encodeConfigRegisters();
			this->writeConfigRegisters();
		}
		txdelay = 100;					// TODO replace txdelay with wait loop; DR=HIGH after transmission
	}

	done = false;
	startTime = millis();
	while (((millis() - startTime) < 200) && (done == false)) {	// TODO figure out what timeout we want
		if (board->readPin(PIN_CD) == LOW) {			// Collision detection
			this->setModeTx();
			delay(txdelay);
			this->setModeRx();
			done = true;
		}
	}

	return done;
}

