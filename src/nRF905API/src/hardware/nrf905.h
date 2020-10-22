/*
  nRF905 Class
  written by Eelco Huininga 2020
*/

#ifndef __NRF905_H__
#define __NRF905_H__

#include "../../board.h"	// PIN_CD

#define MAX_TRANSMIT_TIME 2000	// TODO figure out what timeout we want
#define CARRIERDETECT_LED_DELAY	20				// On-board LED will light up for 20ms when data is received

/* nRF905 register sizes */
#define NRF905_REGISTER_COUNT		10
#define NRF905_MAX_FRAMESIZE		32

/* nRF905 Instructions */
#define NRF905_COMMAND_W_CONFIG	0x00
#define NRF905_COMMAND_R_CONFIG	0x10
#define NRF905_COMMAND_W_TX_PAYLOAD	0x20
#define NRF905_COMMAND_R_TX_PAYLOAD	0x21
#define NRF905_COMMAND_W_TX_ADDRESS	0x22
#define NRF905_COMMAND_R_TX_ADDRESS	0x23
#define NRF905_COMMAND_R_RX_PAYLOAD	0x24
#define NRF905_COMMAND_CHANNEL_CONFIG	0x80

typedef union {
	uint8_t		buffer[NRF905_MAX_FRAMESIZE + 1];
	struct {
		uint8_t	command;
		uint8_t	payload[NRF905_MAX_FRAMESIZE];
	};
} nRF905Buffer;

typedef union {
	uint8_t		buffer[NRF905_REGISTER_COUNT + 1];
	struct {
		uint8_t	command;
		uint8_t	config[NRF905_REGISTER_COUNT];
	};
} nRF905ConfigBuffer;

typedef union {
	uint8_t	buffer[5];
	struct {
		uint8_t	command;
		uint8_t	address[4];
	};
} nRF905AddressBuffer;

typedef enum {
	PowerDown = 0,
	Idle,
	Receive,
	Transmit
} Mode;

class nRF905 {
	public:
				/* Function declarations */
				nRF905(uint8_t am, uint8_t cd, uint8_t ce, uint8_t dr, uint8_t pwr, uint8_t txen, uint8_t spi_cs, uint32_t SPIFrequency);
				~nRF905(void);
		bool		init(const uint32_t xtal_frequency, const uint32_t clk_out_frequency, const bool clk_out_enable);
		bool		testSPI(void);
		void		update(void);
		uint8_t	getStatus(void);
		uint32_t	getFrequency(void);
		bool		setFrequency(const uint32_t frequency);
		uint8_t	getTxPower(void);
		bool		setTxPower(const int8_t power);
		bool		getRxReducedPower(void);
		void		setRxReducedPower(const bool power);
		bool		getAutoRetransmit(void);
		void		setAutoRetransmit(const bool retransmit);
		uint8_t	getRxAddressWidth(void);
		bool		setRxAddressWidth(const int8_t width);
		uint8_t	getTxAddressWidth(void);
		bool		setTxAddressWidth(const int8_t width);
		uint8_t	getTxPayloadWidth(void);
		bool		setTxPayloadWidth(const int8_t width);
		uint8_t	getRxPayloadWidth(void);
		bool		setRxPayloadWidth(const int8_t width);
		uint32_t	getRxAddress(void);
		bool		setRxAddress(const int32_t address);
		uint32_t	getTxAddress(void);
		bool		setTxAddress(const int32_t address);
		bool		getCRC(void);
		void		setCRC(const bool crc);
		uint8_t	getCRCbits(void);
		bool		setCRCbits(const uint8_t crc_bits);
		uint32_t	getXtalFrequency(void);
		bool		setXtalFrequency(const uint32_t frequency);
		bool		getClkOut(void);
		void		setClkOut(const bool extclock);
		uint32_t	getClkOutFrequency(void);
		bool		setClkOutFrequency(const uint32_t frequency);
		uint8_t	getMode(void);
		void		setMode(const uint8_t mode);
		void		setModePowerDown(void);
		void		setModeIdle(void);
		void		setModeReceive(void);
		void		setModeTransmit(void);
		void		decodeConfigRegisters(void);
		void		encodeConfigRegisters(void);
		void		readConfigRegisters(void);
		bool		writeConfigRegisters(void);
		void		restoreConfigRegisters(const uint8_t *buffer);
		void		backupConfigRegisters(uint8_t *buffer);
		void		readTxPayload(uint8_t * buffer);
		void		writeTxPayload(const uint8_t * buffer);
		void		readTxAddress(void);
		void		writeTxAddress(void);
		void		readRxPayload(uint8_t * buffer);
		bool		startTx(const uint32_t retransmit, const uint8_t mode);
		static void	startRxISR(void);
		static void	startTxISR(void);
#ifdef PIN_CD
		static void	carrierDetectISR(void);
#endif

				/* Variable declarations */
		static volatile bool 		tx_frame_done;
		static volatile uint32_t	tx_retransmit_count;
		static volatile unsigned long	startCDLED;			// Time when the on-board LED was turned on

	private:
		uint8_t	_am;				// Board hardware pin of the AM connection
		uint8_t	_cd;				// Board hardware pin of the CD connection
		uint8_t	_ce;				// Board hardware pin of the CE connection
		uint8_t	_spi_cs;			// Board hardware pin of the SPI_CS connection
		uint8_t	_dr;				// Board hardware pin of the DR connection
		uint8_t	_pwr;				// Board hardware pin of the PWR connection
		uint8_t	_txen;				// Board hardware pin of the TXEN connection

		uint32_t	_spi_frequency;		// SPI frequency

		char *		_board;			// Pointer to class with board specific function calls

		uint8_t	_status;			// nRF905 status register, stored from last operation
		uint8_t	_mode;				// Internal: variable
		bool		_band;				// nRF905 href_ppl: false=434MHz band, true=868MHZ band
		uint16_t	_channel;			// nRF905 RF channel
		uint32_t	_frequency;			// Internal: RF frequency (internal use; not nRF905 register)
		bool		_rx_power;			// nRF905 Receive power: false=normal, true=reduced
		int8_t		_tx_power;			// nRF905 Transmit power (-10dBm, -2dBm, 6dBm or 10dBm)
		uint8_t	_rx_address_width;		// nRF905 Receive address size (1-4 bytes)
		uint8_t	_tx_address_width;		// nRF905 Transmit address size (1-4 bytes)
		uint8_t	_rx_payload_width;		// nRF905 Receive payload size (1-32 bytes)
		uint8_t	_tx_payload_width;		// nRF905 Transmit payload size (1-32 bytes)
		uint32_t	_rx_address;			// nRF905 Receive address
		uint32_t	_tx_address;			// nRF905 Transmit address
		bool		_crc_enable;			// nRF905 Enable CRC: false=CRC disabled, true=CRC enabled
		uint8_t	_crc_bits;			// nRF905 CRC size: 8=8bit CRC, 16=16bit CRC
		bool		_auto_retransmit;		// nRF905 Auto retransmission flag: false=off, true=on
		uint32_t	_clk_out_frequency;		// nRF905 clock out frequency
		bool		_clk_out_enable;		// nRF905 clock out enabled: false=off, true=on
		uint32_t	_xtal_frequency;		// nRF905 clock in frequency
		nRF905ConfigBuffer	_config_registers;	// Internal: Cached copy of the nRF905's config registers
		nRF905Buffer	_rx_payload;			// nRF905 Rx Payload
		nRF905Buffer	_tx_payload;			// nRF905 Tx Payload
};

extern nRF905 * nrf905;

void txISR(void);
#ifdef PIN_CD
void cdISR(void);
#endif


#endif

