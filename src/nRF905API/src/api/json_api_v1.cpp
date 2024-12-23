#include "../../fan.h"						// Fan class
#include "../../nvram.h"					// 
#include "../../utils.h"					// binToHexString()
#include "../html/html.h"					// HTTP_CODE_*
#include "json_api.h"						// json_sendReply(), HTTP_HTML_AUTH_REALM, HTTP_JSON_RESPONSE
#include "../hardware/nrf905.h"				// NRF905_REGISTER_COUNT, nrf905->*
#include "../language/language.h"				// MSG_API_*
#include "../../nRF905API.h"					// config.*

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config.*: Defined in nRF905API.ino
extern uint8_t				txbuffer[NRF905_MAX_FRAMESIZE];	// Defined in nRF905API.ino
extern uint8_t				rxbuffer[][NRF905_MAX_FRAMESIZE];	// Defined in nRF905API.ino
extern bool				rx_buffer_overflow;	// Defined in nRF905API.ino
extern size_t				rxnum;			// Defined in nRF905API.ino

void apiv1_AddHandlers(void) {
	server.on(FPSTR(API_V1_CONFIG_URL),			HTTP_GET, apiv1_config);
	server.on(FPSTR(API_V1_RECEIVE_URL),			HTTP_GET, apiv1_receive);
	server.on(FPSTR(API_V1_SEND_URL),			HTTP_GET, apiv1_send);
	server.on(FPSTR(API_V1_STATUS_URL),			HTTP_GET, apiv1_status);
	server.on(FPSTR(API_V1_SYSTEMCONFIG_URL),		HTTP_GET, apiv1_systemConfig);
#if NRF905API_FAN == 1
	server.on(FPSTR(API_V1_ZEHNDER_CONFIG_URL),		HTTP_GET, json_sendDeprecated);
	server.on(FPSTR(API_V1_ZEHNDER_LINK_URL),		HTTP_GET, json_sendDeprecated);
	server.on(FPSTR(API_V1_ZEHNDER_SETPOWER_URL),		HTTP_GET, json_sendDeprecated);
	server.on(FPSTR(API_V1_ZEHNDER_SETTIMER_URL),		HTTP_GET, json_sendDeprecated);
#endif
}

/* Handler for the API v1 config call */
void apiv1_config() {
	uint8_t	txaddrwidth,
			rxaddrwidth,
			txpayloadwidth,
			rxpayloadwidth;
	int8_t		txpower;
	uint32_t	frequency,
			rxaddr,
			txaddr;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(HTTP_HTML_AUTH_FAILED));
	}

	/* Check if a frequency parameter was given */
	if (server.hasArg(MSG_API_FREQUENCY)) {
		/* Check if the frequency parameter is valid */
		if (!isInt(server.arg(MSG_API_FREQUENCY).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'frequency' is not valid integer");
			return;
		}
		frequency = server.arg(MSG_API_FREQUENCY).toInt();
		if (nrf905->setFrequency(frequency) == false) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Failed to set frequency");
			return;
		}
	}

	/* Check if crc parameter was given */
	if (server.hasArg(MSG_API_CRC)) {
		if (server.arg(MSG_API_CRC) == MSG_API_OFF) {
			nrf905->setCRC(false);
		} else if (server.arg(MSG_API_CRC) == MSG_API_8) {
			nrf905->setCRC(true);
			nrf905->setCRCbits(8);
		} else if (server.arg(MSG_API_CRC) == MSG_API_16) {
			nrf905->setCRC(true);
			nrf905->setCRCbits(16);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'crc' is not valid");
			return;
		}		
	}

	/* Check if txpower parameter was given */
	if (server.hasArg(MSG_API_TXPOWER)) {
		/* Check if the txpower parameter is valid */
		if (!isInt(server.arg(MSG_API_TXPOWER).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txpower' is not valid integer");
			return;
		}
		txpower = server.arg(MSG_API_TXPOWER).toInt();	// TODO: do some checks to see if txpower is within int8_t type (-128...127)
		if (nrf905->setTxPower(txpower) == false) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txpower' is not valid");
			return;
		}		
	}

	/* Check if rxpower parameter was given */
	if (server.hasArg(MSG_API_RXPOWER)) {
		if (server.arg(MSG_API_RXPOWER) == MSG_API_NORMAL) {
			nrf905->setRxReducedPower(false);
		} else if (server.arg(MSG_API_RXPOWER) == MSG_API_LOW) {
			nrf905->setRxReducedPower(true);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'rxpower' is not valid");
			return;
		}		
	}

	/* Check if txaddrwidth parameter was given */
	if (server.hasArg(MSG_API_TXADDRWIDTH)) {
		txaddrwidth = server.arg(MSG_API_TXADDRWIDTH).toInt();
		if (txaddrwidth == 1) {
			nrf905->setTxAddressWidth(1);
		} else if (txaddrwidth == 4) {
			nrf905->setTxAddressWidth(4);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txaddrwidth' is not valid");
			return;
		}		
	}

	/* Check if rxaddrwidth parameter was given */
	if (server.hasArg(MSG_API_RXADDRWIDTH)) {
		rxaddrwidth = server.arg(MSG_API_RXADDRWIDTH).toInt();
		if (rxaddrwidth == 1) {
			nrf905->setRxAddressWidth(1);
		} else if (rxaddrwidth == 4) {
			nrf905->setRxAddressWidth(4);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'rxaddrwidth' is not valid");
			return;
		}		
	}

	/* Check if txpayloadwidth parameter was given */
	if (server.hasArg(MSG_API_TXPAYLOADWIDTH)) {
		txpayloadwidth = server.arg(MSG_API_TXPAYLOADWIDTH).toInt();
		if ((txpayloadwidth > 0) || (txpayloadwidth < 33)) {
			nrf905->setTxPayloadWidth(txpayloadwidth);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txpayloadwidth' is not valid");
			return;
		}		
	}

	/* Check if rxpayloadwidth parameter was given */
	if (server.hasArg(MSG_API_RXPAYLOADWIDTH)) {
		rxpayloadwidth = server.arg(MSG_API_RXPAYLOADWIDTH).toInt();
		if ((rxpayloadwidth > 0) || (rxpayloadwidth < 33)) {
			nrf905->setRxPayloadWidth(rxpayloadwidth);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'rxpayloadwidth' is not valid");
			return;
		}		
	}

	/* Check if txaddr parameter was given */
	if (server.hasArg(MSG_API_TXADDR)) {
		txaddr = strtoul(server.arg(MSG_API_TXADDR).c_str(), 0, 16);

		if (nrf905->getTxAddressWidth() == 4) {
			nrf905->setTxAddress(txaddr);
			nrf905->writeTxAddress();
		} else if ((nrf905->getTxAddressWidth() == 1) && (txaddr < 256)) {
			nrf905->setTxAddress(txaddr);
			nrf905->writeTxAddress();
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txaddr' is not valid");
			return;
		}		
	}

	/* Check if rxaddr parameter was given */
	if (server.hasArg(MSG_API_RXADDR)) {
		rxaddr = strtoul(server.arg(MSG_API_RXADDR).c_str(), 0, 16);

		if (nrf905->getRxAddressWidth() == 4) {
			nrf905->setRxAddress(rxaddr);
		} else if ((nrf905->getRxAddressWidth() == 1) && (rxaddr < 256)) {
			nrf905->setRxAddress(rxaddr);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'rxaddr' is not valid");
			return;
		}		
	}

	/* Write config to the nRF905 chip */
	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->encodeConfigRegisters();
	nrf905->writeConfigRegisters();
	nrf905->setModeReceive();	// Enable Rx mode on the nRF905

	/* Write to NVRAM, if requested */
	if (server.arg(MSG_API_NVRAM) == MSG_API_TRUE) {
		writeNVRAM();
	}

	/* Write HTTP result */
	json_sendReply(HTTP_CODE_OK, MSG_API_OK, "");
}

/* Handler for the API v1 receive call */
void apiv1_receive() {
	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	if (rx_buffer_overflow == false)
		json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	else
		json_sendHeader(HTTP_CODE_OK, MSG_API_RX_BUFFER_OVERFLOW, "");

	json_sendRxData();
	json_sendFooter();
}

/* Handler for the API v1 send call */
void apiv1_send() {
	uint32_t	txaddr,
			retransmit,
			timeout;
	size_t		payload_size,
			rxnum_before;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the addr parameter is present */
	if (!server.hasArg(MSG_API_TXADDR)) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txaddr' missing");
		return;
	} else {
		/* Check if the txaddr parameter is valid */
		if (!isHex(server.arg(MSG_API_TXADDR).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'txaddr' is not valid hex");
			return;
		}
		txaddr = strtoul(server.arg(MSG_API_TXADDR).c_str(), 0, 16);
	}

	/* Check if the payload parameter is present */
	if (!server.hasArg("payload")) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'payload' missing");
		return;
	} else {
		/* Check if the payload parameter is valid */
		if (!isHex(server.arg(MSG_API_PAYLOAD).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'payload' is not valid hex");
			return;
		}
		payload_size = hexstringToBin(server.arg(MSG_API_PAYLOAD).c_str(), txbuffer, sizeof(txbuffer));
		if (payload_size < 0) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Can't convert 'payload' to hex");
			return;
		}
	}

	/* Check if a retransmit parameter was given */
	if (server.hasArg(MSG_API_RETRANSMIT)) {
		/* Check if the retransmit parameter is valid */
		if (!isInt(server.arg(MSG_API_RETRANSMIT).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'retransmit' is not valid integer");
			return;
		}
		retransmit = server.arg(MSG_API_RETRANSMIT).toInt();
	} else
		retransmit = FAN_TX_FRAMES;		// No retransmission by default

	/* Check if a timeout parameter was given */
	if (server.hasArg(MSG_API_TIMEOUT)) {
		/* Check if the timeout parameter is valid */
		if (!isInt(server.arg(MSG_API_TIMEOUT).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'timeout' is not valid integer");
			return;
		}
		timeout = server.arg(MSG_API_TIMEOUT).toInt();
	} else
		timeout = FAN_REPLY_TIMEOUT;		// 500ms default

	/* Set transmit address, if nessecary */
	if (txaddr != nrf905->getTxAddress()) {
		nrf905->setTxAddress(txaddr);
		nrf905->writeTxAddress();
	}

	/* Transmit the payload */
	nrf905->writeTxPayload(txbuffer);
	if (nrf905->startTx(retransmit, Receive) == true) {
		json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
		do {
			rxnum_before = rxnum;
			delay(timeout);
		} while (rxnum_before != rxnum);	// Wait so we can receive all the data
	} else {
		json_sendHeader(HTTP_CODE_OK, MSG_API_ERROR, MSG_API_NETWORK_BLOCKED);
	}

	json_sendRxData();
	json_sendFooter();
}

/* Handler for the API v1 status call */
void apiv1_status() {
	uint8_t	payloadbin[NRF905_MAX_FRAMESIZE],
			nvrambuffer[NVRAM_SIZE];
	char		payloadstr[(NRF905_MAX_FRAMESIZE * 2) + 1] = {0},
			nvramstr[(NVRAM_SIZE * 2) + 1],
			hexstr[3];

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	server.sendContent(",\"system_status\":{");
	server.sendContent("\"board\":\"" + board->get_arch() + "\",");
	server.sendContent("\"firmware_version\":\"" + String(FPSTR(firmware_version)) + "\",");
	server.sendContent("\"restart_reason\":" + String(board->restartReason()) + ",");
	server.sendContent("\"cpu_id\":\"" + String(board->get_cpu_id(), HEX) + "\",");
	server.sendContent("\"cpu_frequency\":" + String(board->getCPUFreqMhz()) + ",");
	server.sendContent("\"vcc\":" + String(board->get_vcc() / 1000.0) + ",");
	server.sendContent("\"sdk_version\":\"" + String(board->getSdkVersion()) + "\",");
	server.sendContent("\"core_version\":\"" + board->get_core_version() + "\",");
	server.sendContent("\"flash_chip_id\":\"" + String(board->get_flash_chip_id(), HEX) + "\",");
	server.sendContent("\"flash_chip_speed\":" + String(board->getFlashChipSpeed()) + ",");
	server.sendContent("\"flash_mode\":\"" + String(board->getFlashMode()) + "\",");
	server.sendContent("\"flash_chip_real_size\":" + String(board->get_flash_chip_real_size()) + ",");
	server.sendContent("\"flash_chip_sdk_size\":" + String(board->getFlashChipSdkSize()) + ",");
	server.sendContent("\"flash_crc\":" + String(board->checkFlashCRC() ? "true" : "false" ) + ",");
	server.sendContent("\"sketch_size\":" + String(board->getSketchSize()) + ",");
	server.sendContent("\"sketch_free_space\":" + String(board->getFreeSketchSpace()) + ",");
	server.sendContent("\"sketch_md5\":\"" + board->getSketchMD5() + "\",");
	server.sendContent("\"heap_free\":" + String(board->getFreeHeap()) + ",");
	server.sendContent("\"heap_fragmentation\":" + String(board->getHeapFragmentation()) + ",");
	server.sendContent("\"heap_maxfreeblocksize\":" + String(board->getHeapMaxFreeBlockSize()) + ",");
	board->ReadNVRAM(nvrambuffer, 0, NVRAM_SIZE);
	strcpy(nvramstr, "n/a");
//	binToHexstring(nvrambuffer, nvramstr, sizeof(nvramstr));
	server.sendContent("\"nvram\":\"" + String(nvramstr) + "\"}");
//#include <WiFiNINA.h>
//	server.sendContent("\"wifi_firmware_version\":\"" +  WiFi.firmwareVersion() + "\"}");

	server.sendContent(",\"network\":{");
	server.sendContent("\"mac\":\"" + String(WiFi.macAddress()) + "\",");
	server.sendContent("\"hostname\":\"" + board->get_hostname() + "\",");
	server.sendContent("\"ip4_addr\":\"" + WiFi.localIP().toString() + "\",");
	server.sendContent("\"ip4_subnet\":\"" + WiFi.subnetMask().toString() + "\",");
	server.sendContent("\"ip4_gateway\":\"" + WiFi.gatewayIP().toString() + "\",");
	server.sendContent("\"ip4_dns\":\"" + WiFi.dnsIP().toString() + "\",");
	server.sendContent("\"ip6_addr\":\"" + board->get_localIPv6() + "\",");
	server.sendContent("\"rssi\":" + String(WiFi.RSSI()) + ",");
	server.sendContent("\"ssid\":\"" + WiFi.SSID() + "\",");
	server.sendContent("\"bssid\":\"" + WiFi.BSSIDstr() + "\",");
	server.sendContent("\"channel\":" + String(WiFi.channel()) + "}");

	server.sendContent(",\"nrf905\":{");
	nrf905->readConfigRegisters();
	nrf905->decodeConfigRegisters();
	if (nrf905->testSPI() == true)
		server.sendContent("\"status\":\"ok\",");
	else
		server.sendContent("\"status\":\"failed\",");
	uint8ToString(nrf905->getStatus(), hexstr);
	server.sendContent("\"status_register\":\"" + String(hexstr) + "\",");
	server.sendContent("\"mode\":" + String(nrf905->getMode()) + ",");
	server.sendContent("\"clock_frequency\":" + String(nrf905->getXtalFrequency()) + ",");
	server.sendContent("\"clk_out_enabled\":\"" + String(nrf905->getClkOut() ? "true" : "false" ) + "\",");
	server.sendContent("\"clk_out_frequency\":" + String(nrf905->getClkOutFrequency()) + ",");
	server.sendContent("\"rf_frequency\":" + String(nrf905->getFrequency()) + ",");
	server.sendContent("\"tx_power\":" + String(nrf905->getTxPower()) + ",");
	server.sendContent("\"rx_reduced_power\":" + String(nrf905->getRxReducedPower() ? "true" : "false" ) + ",");
	server.sendContent("\"tx_auto_retransmit\":" + String(nrf905->getAutoRetransmit() ? "true" : "false" ) + ",");
	server.sendContent("\"rx_address_width\":" + String(nrf905->getRxAddressWidth()) + ",");
	server.sendContent("\"tx_address_width\":" + String(nrf905->getTxAddressWidth()) + ",");
	server.sendContent("\"rx_payload_width\":" + String(nrf905->getRxPayloadWidth()) + ",");
	server.sendContent("\"tx_payload_width\":" + String(nrf905->getTxPayloadWidth()) + ",");
	server.sendContent("\"rx_address\":\"" + String(nrf905->getRxAddress(), HEX) + "\",");
	server.sendContent("\"tx_address\":\"" + String(nrf905->getTxAddress(), HEX) + "\",");
	nrf905->readRxPayload(payloadbin);
	binToHexstring(payloadbin, payloadstr, sizeof(payloadbin));
	server.sendContent("\"rx_payload\":\"" + String(payloadstr) + "\",");
	nrf905->readTxPayload(payloadbin);
	binToHexstring(payloadbin, payloadstr, sizeof(payloadbin));
	server.sendContent("\"tx_payload\":\"" + String(payloadstr) + "\",");
	server.sendContent("\"rx_buffered_frames\":" + String(rxnum) + ",");
	server.sendContent("\"crc\":" + String(nrf905->getCRC() ? "true" : "false" ) + ",");
	server.sendContent("\"crc_bits\":" + String(nrf905->getCRCbits()) + "}");

	server.sendContent(",\"fanconfig\":{");
	server.sendContent("\"network\":\"" + String(config.fan_network_id, HEX) + "\",");
	server.sendContent("\"my_device_type\":\"" + String(config.fan_my_device_type, HEX) + "\",");
	server.sendContent("\"my_device_id\":\"" + String(config.fan_my_device_id, HEX) + "\",");
	server.sendContent("\"remote_type\":\"" + String(config.fan_main_unit_type, HEX) + "\",");
	server.sendContent("\"remote_id\":\"" + String(config.fan_main_unit_id, HEX) + "\"}");

	json_sendFooter();
}

void apiv1_systemConfig() {
	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the NVRAM needs to be cleared */
	if (server.hasArg(MSG_API_NVRAM)) {
		if (server.arg(MSG_API_NVRAM) == FPSTR(MSG_API_CLEAR)) {
			/* Clear the NVRAM */
			clearNVRAM();
			board->CommitNVRAM();
		}
		else if (server.arg(MSG_API_NVRAM) == FPSTR(MSG_API_SAVE)) {
			/* Write current settings to NVRAM */
			writeNVRAM();
			board->CommitNVRAM();
		}
		else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'nvram' is not valid");
			return;
		}
	}

	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	json_sendFooter();

	/* Check if the MCU needs to be reset */
	if (server.arg(MSG_API_RESET) == MSG_API_TRUE) {
		delay(250);
		nrf905->setModeIdle();
		board->reset();
	}
}

