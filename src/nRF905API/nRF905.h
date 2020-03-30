/*
  nRF905 Class
  written by Eelco Huininga 2020
*/

/* nRF905 Instructions */
#define NRF905_COMMAND_W_CONFIG	0x00
#define NRF905_COMMAND_R_CONFIG	0x10
#define NRF905_COMMAND_W_TX_PAYLOAD	0x20
#define NRF905_COMMAND_R_TX_PAYLOAD	0x21
#define NRF905_COMMAND_W_TX_ADDRESS	0x22
#define NRF905_COMMAND_R_TX_ADDRESS	0x23
#define NRF905_COMMAND_R_RX_PAYLOAD	0x24
#define NRF905_COMMAND_CHANNEL_CONFIG	0x80

/* NVRAM checksum */
#define NRF905_NVRAM_OFFSET		0x0800		// First 2048 bytes are for the system, so start at 2048 (0x0800)
#define NRF905_NVRAM_CHECKSUM		0xE5

typedef union {
	uint8_t	buffer[33];
	struct {
		uint8_t	command;
		uint8_t	payload[32];
	};
} nRF905Buffer;

typedef union {
	uint8_t	buffer[11];
	struct {
		uint8_t	command;
		uint8_t	config[10];
	};
} nRF905ConfigBuffer;

typedef union {
	uint8_t	buffer[5];
	struct {
		uint8_t	command;
		uint8_t	address[4];
	};
} nRF905AddressBuffer;

typedef struct {
	uint16_t		signature;		// Always 0xF905
	uint8_t		config[10];		// nRF905 config registers
	uint8_t		tx_payload[32];	// nRF905 Tx payload
	uint32_t		tx_address;		// nRF905 Tx address
	uint8_t		checksum;
} nRF905NVRAMBuffer;

class nRF905 {
	public:
		typedef enum {
			PowerDown = 0,
			Idle,
			Receive,
			Transmit
		} Mode;

				nRF905(uint8_t am, uint8_t cd, uint8_t ce, uint8_t dr, uint8_t pwr, uint8_t txen, uint8_t spi_cs, uint32_t SPIFrequency);
				~nRF905(void);
		bool		init(const uint32_t xtal_frequency, const uint32_t clk_out_frequency, const bool clk_out_enable);
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
		void		setModePowerDown(void);
		void		setModeIdle(void);
		void		setModeReceive(void);
		void		setModeTransmit(void);
		void		decodeConfigRegisters(void);
		void		encodeConfigRegisters(void);
		void		readConfigRegisters(void);
		bool		writeConfigRegisters(void);
		void		readTxPayload(uint8_t * buffer);
		void		writeTxPayload(const uint8_t * buffer);
		void		readTxAddress(void);
		void		writeTxAddress(void);
		void		readRxPayload(uint8_t * buffer);
		void		channelConfig(void);
		bool		startTx(uint32_t retransmit, uint32_t timeout);
		bool		readNVRAM(void);
		void		writeNVRAM(void);
		void		clearNVRAM(void);

	private:
		uint8_t	_am;
		uint8_t	_cd;
		uint8_t	_ce;
		uint8_t	_spi_cs;
		uint8_t	_dr;
		uint8_t	_pwr;
		uint8_t	_txen;

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
		nRF905ConfigBuffer	_config_registers;		// Internal: Cached copy of the nRF905's config registers
		nRF905Buffer	_rx_payload;			// nRF905 Rx Payload
		nRF905Buffer	_tx_payload;			// nRF905 Tx Payload
};

