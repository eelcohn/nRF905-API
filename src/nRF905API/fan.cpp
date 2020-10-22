#include <cstdint>						// uint8_t, uint16_t, uint32_t
#include "board.h"						// board->*
#include "fan.h"
#include "nvram.h"						// NVRAMBuffer
#include "utils.h"			
#include "nRF905API.h"						// rxbuffer, rxnum, config.*
#include "src/language/language.h"				// MSG_JSON_*
#include "src/hardware/nrf905.h"				// nrf905->

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config: Defined in nRF905API.ino
extern size_t				rxnum;			// rxnum: Defined in nRF905API.ino
extern unsigned long			startTime;		// startTimeL Defined in nRF905API.ino
extern uint8_t				rxbuffer[][NRF905_MAX_FRAMESIZE];

/* Constructor for Fan class */
Fan::Fan(void) {
	size_t		i;

	/* Reset all known devices to 0x00 (not set/broadcast) */
	for (i = 0; i < sizeof(this->known_devices); i++) {
		known_devices[i] = FAN_TYPE_BROADCAST;
	}
}

/* Deconstructor for Fan class */
Fan::~Fan(void) {
}

size_t Fan::discover(const uint8_t device_id, const uint32_t timeout) {
	uint8_t	frametype,
			device_type = FAN_TYPE_REMOTE_CONTROL,
			rx_type,
			tx_type = 0x00,
			rx_id,
			tx_id = 0x00;
	uint32_t	network_id = 0x00000000;
	size_t		i,
			result;

	uint8_t payload[FAN_FRAMESIZE] = {0x04, 0x00, device_type, device_id, FAN_TTL, FAN_NETWORK_JOIN_ACK, 0x04, 0xa5, 0x5a, 0x5a, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00};

	Serial.println("Debug: My ID is 0x"+(String(device_id, HEX))+". Searching...");

	/* Set transmit and receive network address to Link address */
	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->setRxAddress(network_link_id);
	nrf905->setTxAddress(network_link_id);
	nrf905->writeTxAddress();
	nrf905->encodeConfigRegisters();
	nrf905->writeConfigRegisters();

	rxnum = 0;						// TODO: display warning in case there's still unprocessed rxdata
	result = FAN_ERROR_NOT_FOUND;
	nrf905->writeTxPayload(payload);
	if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
		for (i = 0; i < rxnum; i++) {			// Process all received replies
			rx_type = rxbuffer[i][0x00];
			rx_id = rxbuffer[i][0x01];
			tx_type = rxbuffer[i][0x02];
			tx_id = rxbuffer[i][0x03];
			frametype = rxbuffer[i][0x05];	// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case FAN_NETWORK_JOIN_OPEN :	// Received linking request from main unit
					if (result != FAN_RESULT_SUCCESS) {
						for (int t=0; t < 16; t++) {
							Serial.printf("%02X:", rxbuffer[i][t]);
						}

						network_id = (rxbuffer[i][0x07] | (rxbuffer[i][0x08] << 8) | (rxbuffer[i][0x09] << 16) | (rxbuffer[i][0x0A] << 24));
						result = FAN_RESULT_SUCCESS;
					}
					break;

				default :
					Serial.println("Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
					break;
			}
		}
	} else {
		result = FAN_ERROR_CONFIG_FAILED;
	}

	/* Found a main unit, so send a join request */
	if (result == FAN_RESULT_SUCCESS) {
		Serial.printf("discover: Found unit type 0x%02X with ID 0x%02X on network 0x%08X\n", tx_type, tx_id, network_id);
		payload[0x00] = FAN_TYPE_MAIN_UNIT;			// Set type to main unit
		payload[0x01] = tx_id;					// Set ID to the ID of the main unit
		payload[0x05] = FAN_NETWORK_JOIN_REQUEST;		// Request to connect to network
		payload[0x07] = (network_id & 0x000000FF);		// Request to connect to the received network ID
		payload[0x08] = (network_id & 0x0000FF00) >> 8;
		payload[0x09] = (network_id & 0x00FF0000) >> 16;
		payload[0x0A] = (network_id & 0xFF000000) >> 24;
		nrf905->setRxAddress(network_id);
		nrf905->setTxAddress(network_id);
		nrf905->writeTxAddress();
		nrf905->encodeConfigRegisters();
		nrf905->writeConfigRegisters();
		nrf905->writeTxPayload(payload);
		for (int t=0; t < 16; t++) {
			Serial.printf("%02X:", payload[t]);
		}
		rxnum = 0;						// TODO: display warning in case there's still unprocessed rxdata
		result = FAN_REPLY_TIMEOUT;
		if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
			config.fan_main_unit_type = tx_type;
			config.fan_main_unit_id = tx_id;
			config.nrf905_tx_address = network_id;
			for (i = 0; i < rxnum; i++) {			// Process all received replies
				rx_type = rxbuffer[i][0x00];
				rx_id = rxbuffer[i][0x01];
				tx_type = rxbuffer[i][0x02];
				tx_id = rxbuffer[i][0x03];
				frametype = rxbuffer[i][0x05];	// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
				switch (frametype) {
					case FAN_FRAME_0B :	// Main unit link was succesful (acknowledge)
						if ((rx_type = device_type) && (rx_id == device_id) && (tx_type == config.fan_main_unit_type) && (tx_id == config.fan_main_unit_id)) {
							if (result != FAN_RESULT_SUCCESS) {
								Serial.println("Debug: Link successful to unit with ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
								result = FAN_RESULT_SUCCESS;
							}
						} else {
							Serial.println("Debug: Received unknown link succes from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
						}
						break;

					default :
						Serial.println("Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
						break;
				}
			}
		} else {
			result = FAN_ERROR_CONFIG_FAILED;
		}
	}

	if (result == FAN_RESULT_SUCCESS) {
		/* Send request to join network */
		Serial.printf("Debug: sending join request acknowledge 0x0b\n");
		payload[0x05] = FAN_FRAME_0B;	// 0x0B acknowledgee link successful
		payload[0x06] = 0x00;	// No parameters
		payload[0x07] = 0x00;	// Clear rest of payload buffer
		payload[0x08] = 0x00;
		payload[0x09] = 0x00;
		payload[0x0A] = 0x00;
		nrf905->writeTxPayload(payload);
		if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
			for (i = 0; i < rxnum; i++) {			// Process all received replies
				if (rxbuffer[i][0x05] == FAN_TYPE_QUERY_NETWORK) {
					if (result != FAN_RESULT_SUCCESS)
						Serial.print("Debug: received network join success 0x0D\n");
					if ((rxbuffer[i][0x00] == tx_type) && (rxbuffer[i][0x01] == tx_id) && (rxbuffer[i][0x02] == tx_type) && (rxbuffer[i][0x03] == tx_id)) {
						if (result != FAN_RESULT_SUCCESS) {
							Serial.printf("Debug: 0x0D sanity check: ok\n");
							result = FAN_RESULT_SUCCESS;
						}
					}
				}
			}
		} else {
			result = FAN_ERROR_CONFIG_FAILED;
		}

		/* */
		if (result == FAN_RESULT_SUCCESS) {
			config.fan_network_id = network_id;
			config.fan_main_unit_type = tx_type;
			config.fan_main_unit_id = tx_id;
			nrf905->setRxAddress(network_id);
			nrf905->setTxAddress(network_id);
			nrf905->writeTxAddress();
			nrf905->encodeConfigRegisters();
			nrf905->writeConfigRegisters();
			Serial.printf("Debug: join successful\n");
		} else {
			Serial.printf("Debug: unable to complete join transaction\n");
		}
	} else {
		Serial.printf("Debug: no networks found\n");
	}

	return result;
}

size_t Fan::setSpeed(const uint8_t speed, const uint8_t timer) {
	uint8_t	frametype,
			rx_type,
			rx_id,
			tx_id;
	uint8_t	payload[FAN_FRAMESIZE] = {FAN_TYPE_MAIN_UNIT, 0x00, config.fan_my_device_type, config.fan_my_device_id, FAN_TTL, FAN_FRAME_SETSPEED, 0x01, speed, timer, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t		i,
			result;

	if (timer != 0) {
		payload[0x05] = FAN_FRAME_SETTIMER;
		payload[0x06] = 0x02;
	}

	/* Transmit the payload */
	result = FAN_ERROR_NO_ACKNOWLEDGE;
	nrf905->writeTxPayload(payload);
	if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
		for (i = 0; i < rxnum; i++) {						// Process all received replies
			rx_type = rxbuffer[i][0x00];
			rx_id = rxbuffer[i][0x01];
			tx_id = rxbuffer[i][0x03];
			frametype = rxbuffer[i][0x05];					// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case FAN_FRAME_SETSPEED :				// Retransmit of command by main unit (mesh networking)
//					Serial.println((String)"Debug: received 0x02");
					break;

				case FAN_FRAME_SETSPEED_REPLY :			// Retransmision from main unit of reply to command 0x05 from RFZ (mesh networking)
//					Serial.println((String)"Debug: received 0x05");
					break;

				case FAN_TYPE_FAN_SETTINGS :			// Reply / info request???
					if ((result != FAN_RESULT_SUCCESS) && (rx_type = config.fan_my_device_type) && (rx_id == config.fan_my_device_id)) {
						Serial.println((String)"Debug: sending reply to 0x07");
						payload[0x05] = FAN_FRAME_SETSPEED_REPLY;	// Reply to 0x07
						payload[0x06] = 0x03;				// 3 parameters
						payload[0x07] = 0x54;				// ???
						payload[0x08] = 0x03;				// ???
						payload[0x09] = 0x20;				// ???
						nrf905->writeTxPayload(payload);
						if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
							result = FAN_RESULT_SUCCESS;
						}
					}
					break;

				default :
					Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(nrf905->getRxAddress(), HEX)));
					break;
			}
		}
	}

	return result;
}

size_t Fan::setVoltage(const uint8_t voltage) {
	uint8_t	frametype,
			rx_type,
			rx_id,
			tx_id;
	uint8_t	payload[FAN_FRAMESIZE] = {FAN_TYPE_MAIN_UNIT, 0x00, config.fan_my_device_type, config.fan_my_device_id, FAN_TTL, FAN_FRAME_SETVOLTAGE, 0x01, voltage, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t		i,
			result;

	/* Transmit the payload */
	result = FAN_ERROR_NO_ACKNOWLEDGE;
	nrf905->writeTxPayload(payload);
	if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
		for (i = 0; i < rxnum; i++) {			// Process all received replies
			rx_type = rxbuffer[i][0x00];
			rx_id = rxbuffer[i][0x01];
			tx_id = rxbuffer[i][0x03];
			frametype = rxbuffer[i][0x05];		// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case FAN_FRAME_SETVOLTAGE :		// 0x01: Retransmit of command by main unit (mesh networking)
//					Serial.println((String)"Debug: received 0x01");
					result = FAN_ERROR_NO_REPLY;
					break;

				case FAN_TYPE_FAN_SETTINGS :			// 0x07: Reply / info request??? TODO replace 0xFF with correct value
					if ((result != FAN_RESULT_SUCCESS) && (rx_type = config.fan_my_device_type) && (rx_id == config.fan_my_device_id)) {
						Serial.println((String)"Debug: sending reply to 0x07");
						payload[0x05] = FAN_FRAME_SETVOLTAGE_REPLY;	// Reply to 0x07
						payload[0x06] = 0x03;				// 3 parameters
						payload[0x07] = 0x76;				// ???
						payload[0x08] = voltage;			// ???
						payload[0x09] = 0x28;				// ???
						nrf905->writeTxPayload(payload);
						if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
							result = FAN_RESULT_SUCCESS;
						}
					}
					break;

				case FAN_FRAME_SETVOLTAGE_REPLY :				// 0x1D: Retransmision from main unit of reply to command 0x1D from RFZ (mesh networking)
//					Serial.println((String)"Debug: received 0x1D");
					break;

				default :
					Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(nrf905->getRxAddress(), HEX)));
					break;
			}
		}
	}

	return result;
}

size_t Fan::queryDevice(const uint8_t id, uint8_t *speed, uint8_t *voltage, uint8_t *timer) {
	uint8_t	frametype,
			rx_type,
			rx_id,
			tx_id;
	uint8_t	payload[FAN_FRAMESIZE] = {FAN_TYPE_MAIN_UNIT, id, config.fan_my_device_type, config.fan_my_device_id, FAN_TTL, FAN_TYPE_QUERY_NETWORK, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t		i,
			result;

	/* Transmit the payload */
	result = FAN_ERROR_NO_ACKNOWLEDGE;
	nrf905->writeTxPayload(payload);
	if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
		for (i = 0; i < rxnum; i++) {			// Process all received replies
			rx_type = rxbuffer[i][0x00];
			rx_id = rxbuffer[i][0x01];
			tx_id = rxbuffer[i][0x03];
			frametype = rxbuffer[i][0x05];		// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case FAN_TYPE_FAN_SETTINGS :			// 0x07: Reply / info request??? TODO replace 0xFF with correct value
					if ((result != FAN_RESULT_SUCCESS) && (rx_type = config.fan_my_device_type) && (rx_id == config.fan_my_device_id)) {
						*speed = rxbuffer[i][0x07];
						*voltage = rxbuffer[i][0x08];
						*timer = rxbuffer[i][0x09];
						result = FAN_RESULT_SUCCESS;
						Serial.printf("Debug: received fan settings; speed=%02X voltage=%i timer=%i\n", *speed, *voltage, *timer);
					}
					break;

				case FAN_TYPE_QUERY_DEVICE :		// 0x10: Retransmit of command by main unit (mesh networking)
//					Serial.println((String)"Debug: received 0x10");
					result = FAN_ERROR_NO_REPLY;
					break;

				default :
					Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(nrf905->getRxAddress(), HEX)));
					break;
			}
		}
	}

	return result;
}

uint8_t Fan::createDeviceID(void) {
	return random(1, 254);		// Generate random device_id; don't use 0x00 and 0xFF
	// TODO: there's a 1 in 255 chance that the generated ID matches the ID of the main unit. Decide how to deal with this (some sort of ping discovery?)
}

size_t Fan::networkScan(const uint32_t start, const uint32_t end) {
	size_t		i,			// network id
			n;			// return value: number of open ports found
	uint8_t	payload[FAN_FRAMESIZE] = {FAN_TYPE_BROADCAST, 0x00, config.fan_my_device_type, config.fan_my_device_id, FAN_TTL, FAN_FRAME_SETSPEED, 0x01, FAN_SPEED_LOW, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	uint8_t rxpayloadbin[NRF905_MAX_FRAMESIZE];
//	char rxpayloadstr[65] = {0};

	/* Transmit the payload */
	nrf905->writeTxPayload(payload);

	n = 0;
	for (i = start; i <= end; i++) {
		nrf905->setTxAddress(i);
		nrf905->writeTxAddress();
		Serial.printf("netscan for %08X ", i);
		if (this->transmitData(1) == FAN_RESULT_SUCCESS) {
			n++;
			Serial.printf("Found network %08X.\f", i);
		}
	}

	return n;
}

size_t Fan::deviceScan(const uint32_t address) {
	size_t		i,			// device id
			retval;		// return value: number of open ports found
	// TODO find a way to do a ping-scan: is there a frame which can be used?
	uint8_t	payload[FAN_FRAMESIZE] = {FAN_TYPE_BROADCAST, 0x00, config.fan_my_device_type, config.fan_my_device_id, FAN_TTL, FAN_FRAME_SETSPEED, 0x01, FAN_SPEED_LOW, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	uint8_t	rxpayloadbin[NRF905_MAX_FRAMESIZE];
//	char		rxpayloadstr[65] = {0};

	nrf905->setTxAddress(address);
	nrf905->writeTxAddress();
	retval = 0;
	for (i = 0; i < 256; i++) {
		payload[0x01] = i;	// Set receiver device ID
		nrf905->writeTxPayload(payload);
		if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
			retval++;
			Serial.printf("Found device %02X in network %08X.\f", i, config.fan_network_id);
		}
	}

	return retval;
}

size_t Fan::portScan(const uint32_t network, const uint8_t to_device_type, const uint8_t to_device_id, const uint8_t from_device_type, const uint8_t from_device_id) {
	size_t		i,			// fan command
			j,			// number of parameters
			retval;		// return value: number of open ports found
	// TODO find a way to do a port scan
	uint8_t	payload[FAN_FRAMESIZE] = {to_device_type, to_device_id, from_device_type, from_device_id, FAN_TTL, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	uint8_t rxpayloadbin[NRF905_MAX_FRAMESIZE];
//	char rxpayloadstr[65] = {0};

	retval = 0;
	for (i = 0; i < 256; i++) {			// Port/command
		for (j = 0; j < 9; j++) {		// Number of parameters
			payload[0x05] = i;		// Set command
			payload[0x06] = j;		// Set number of parameters
			nrf905->writeTxPayload(payload);
			if (this->transmitData(FAN_TX_RETRIES) == FAN_RESULT_SUCCESS) {
				retval++;
				Serial.printf("portScan: device_id: %02X found open port %02X (param length: %02X)\n", to_device_id, i, j);
			}
		}
	}

	return retval;
}

void Fan::test_retransmit(const bool block) {
	if (block == true)
		board->writePin(PIN_TXEN, HIGH);
//		nrf905->setModeTransmit();
	else
		board->writePin(PIN_TXEN, LOW);
//		nrf905->setModeIdle();
}

size_t Fan::transmitData(size_t retries) {
	size_t		i,
			result,
			rxnum_before,
			rxnum_inbetween;

	i = 1;

	do {
		/* Transmit the payload */
		if (nrf905->startTx(FAN_TX_FRAMES, Receive) == true) {
			/* Wait for acknowledge that join network was successful */
			rxnum_before = rxnum;
			result = FAN_ERROR_NO_REPLY;
			do {
				rxnum_inbetween = rxnum;
				delay(FAN_REPLY_TIMEOUT);
			} while (rxnum_inbetween != rxnum);	// Wait so we can receive all the data

			if (rxnum != rxnum_before) {
				result = FAN_RESULT_SUCCESS;
			}
		} else {
			result = FAN_ERROR_TX_FAILED;
		}
	} while ((result != FAN_RESULT_SUCCESS) && (i++ < retries));

	return result;
}

