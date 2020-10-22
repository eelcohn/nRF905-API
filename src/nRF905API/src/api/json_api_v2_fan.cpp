#include "../../fan.h"						// Fan class, FAN_FREQUENCY_ZEHNDER, FAN_FREQUENCY_BUVA
#include "../../utils.h"					// isHex()
#include "../html/html.h"					// HTTP_CODE_*
#include "json_api.h"						// json_sendReply(), HTTP_JSON_RESPONSE
#include "../hardware/nrf905.h"				// nrf905->*
#include "../language/language.h"				// MSG_API_*
#include "../../nRF905API.h"

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config.*: Defined in nRF905API.ino
extern Fan *				fan;			// fan->*: Defined in nRF905API.ino
extern uint8_t				rxbuffer[][NRF905_MAX_FRAMESIZE];


void apiv2_AddHandlers(void) {
#if NRF905API_FAN == 1
	server.on(FPSTR(API_V2_FAN_CONFIG_URL),		HTTP_GET, apiv2_config);
	server.on(FPSTR(API_V2_FAN_LINK_URL),			HTTP_GET, apiv2_discover);
	server.on(FPSTR(API_V2_FAN_SETVOLTAGE_URL),		HTTP_GET, apiv2_setVoltage);
	server.on(FPSTR(API_V2_FAN_SETSPEED_URL),		HTTP_GET, apiv2_setSpeed);
#endif
}

/*
   Handler for the API v2 fan config call
*/
void apiv2_config(void) {
	bool		nvram = true;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if model parameter was given */
	if (server.hasArg(MSG_API_MODEL)) {
		if (server.arg(MSG_API_MODEL) == MSG_API_ZEHNDER) {
			nrf905->setFrequency(868400000);
		} else if (server.arg(MSG_API_MODEL) == MSG_API_BUVA) {
			nrf905->setFrequency(868200000);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'model' is not valid");
			return;
		}		
	}

	/* Check if network parameter was given */
	if (server.hasArg(MSG_API_NETWORK)) {
		/* Check if the network parameter is valid */
		if (!isHex(server.arg(MSG_API_NETWORK).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'network' is not valid hex");
			return;
		}
		config.fan_network_id = strtoul(server.arg(MSG_API_NETWORK).c_str(), 0, 16);
		nrf905->setTxAddress(config.fan_network_id);
		nrf905->writeTxAddress();
		nrf905->setRxAddress(config.fan_network_id);
	} else {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'network' is missing");
		return;
	}		

	/* Check if the main_unit_id parameter is present */
	if (server.hasArg(MSG_API_MAIN_UNIT_ID)) {
		/* Check if the device_id parameter is valid */
		if (!isHex(server.arg(MSG_API_MAIN_UNIT_ID).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'main_unit_id' is not valid hex");
			return;
		}
		config.fan_main_unit_type = FAN_TYPE_MAIN_UNIT;
		config.fan_main_unit_id = strtoul(server.arg("main_unit_id").c_str(), 0, 16);
	} else {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'main_unit_id' is missing");
		return;
	}

	/* Check if the device_id parameter is present */
	if (server.hasArg(MSG_API_DEVICE_ID)) {
		/* Check if the device_id parameter is valid */
		if (!isHex(server.arg(MSG_API_DEVICE_ID).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_id' is not valid hex");
			return;
		}
		config.fan_my_device_type = FAN_TYPE_REMOTE_CONTROL;
		config.fan_my_device_id = strtoul(server.arg(MSG_API_DEVICE_ID).c_str(), 0, 16);
	} else {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_id' is missing");
		return;
	}

	/* Check if nvram parameter was given */
	if (server.hasArg(MSG_API_NVRAM)) {
		if (server.arg(MSG_API_NVRAM) == MSG_API_TRUE) {
			nvram = true;
		} else if (server.arg(MSG_API_NVRAM) == MSG_API_FALSE) {
			nvram = false;
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'nvram' is not valid");
			return;
		}		
	}

	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->setCRC(true);
	nrf905->setCRCbits(16);
	nrf905->setRxReducedPower(false);
	nrf905->setAutoRetransmit(true);
	nrf905->setTxPower(10);
	nrf905->setRxAddressWidth(4);
	nrf905->setTxAddressWidth(4);
	nrf905->setRxPayloadWidth(16);
	nrf905->setTxPayloadWidth(16);
	nrf905->encodeConfigRegisters();
	nrf905->writeConfigRegisters();
	nrf905->setModeReceive();	// Enable Rx mode on the nRF905

	if (nvram == true) {
		writeNVRAM();
		board->CommitNVRAM();
	}

	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	json_sendRxData();
	json_sendFooter();
}

/*
   Handler for the API v2 link call
*/
void apiv2_discover(void) {
	uint32_t	timeout;
	uint8_t	device_id,
			device_type = FAN_TYPE_REMOTE_CONTROL,
			tx_type = 0x00,
			tx_id = 0x00;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the device_id parameter is present */
	if (server.hasArg(MSG_API_DEVICE_ID)) {
		/* Check if the device_id parameter is valid */
		if (!isHex(server.arg(MSG_API_DEVICE_ID).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_id' is not valid hex");
			return;
		}
		device_id = strtoul(server.arg(MSG_API_DEVICE_ID).c_str(), 0, 16);
	} else {
		device_id = fan->createDeviceID();		// Create a new device ID
	}

	/* Check if a timeout parameter was given */
	if (server.hasArg(MSG_API_TIMEOUT)) {
		/* Check if the timeout parameter is valid */
		if (!isInt(server.arg(MSG_API_TIMEOUT).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'timeout' is not valid integer");
			return;
		}
		timeout = server.arg(MSG_API_TIMEOUT).toInt();
	} else {
		timeout = FAN_JOIN_DEFAULT_TIMEOUT;	// 10 seconds default
	}

	/* Start fan discovery */
	switch (fan->discover(device_id, timeout)) {
		case FAN_RESULT_SUCCESS :
			json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
			server.sendContent(",\"network\":\"" + String(config.fan_network_id, HEX) + "\",");
			server.sendContent(",\"remote_device_type\":\"" + String(tx_type, HEX) + "\",");
			server.sendContent(",\"remote_device_id\":\"" + String(tx_id, HEX) + "\",");
			writeNVRAM();
			board->CommitNVRAM();
			break;

		case FAN_ERROR_NOT_FOUND :
			json_sendHeader(HTTP_CODE_OK, MSG_API_FAILED, MSG_API_NO_NETWORKS_FOUND);
			break;

		case FAN_ERROR_NOT_COMPLETED :
			json_sendHeader(HTTP_CODE_OK, MSG_API_FAILED, "Unable to complete join transaction");
			server.sendContent(",\"network\":\"" + String(config.fan_network_id, HEX) + "\",");
			server.sendContent(",\"remote_device_type\":\"" + String(tx_type, HEX) + "\",");
			server.sendContent(",\"remote_device_id\":\"" + String(tx_id, HEX) + "\",");
			break;

		case FAN_ERROR_CONFIG_FAILED :
			json_sendHeader(HTTP_CODE_OK, MSG_API_ERROR, "Failed to write config to nRF905");
			break;

		default :
			json_sendHeader(HTTP_CODE_OK, MSG_API_ERROR, "Unknown error.");
			Serial.println("Unknown result from fanDiscover. This should not happen. Please contact the developer.");
			break;
	}
	server.sendContent(",\"my_device_type\":\"" + String(device_type, HEX) + "\",");
	server.sendContent(",\"my_device_id\":\"" + String(device_id, HEX) + "\",");

	config.fan_my_device_type = device_type;
	config.fan_my_device_id = device_id;

	json_sendRxData();
	json_sendFooter();
}

/*
   Handler for the API v2 set voltage call
*/
void apiv2_setVoltage(void) {
	uint8_t	voltage;
	size_t		result;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the voltage parameter is present */
	if (!server.hasArg("voltage")) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'voltage' is missing");
		return;
	} else {
		if (isFloat(server.arg("voltage"))) {
			voltage = (server.arg("voltage").toFloat() * 10);
		} else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Invalid value for parameter 'voltage'");
			return;
		}
	}

	/* Send the command to the fan unit */
	result = fan->setVoltage(voltage);
	switch (result) {
		case FAN_RESULT_SUCCESS :
			json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
			Serial.printf("setVoltage: ok\n");
			break;

		default :
			json_sendHeader(HTTP_CODE_OK, MSG_API_NO_REPLY, "Command sent but no acknowledge was received");
			Serial.printf("setVoltage: returned %i\n", result);
			break;
	}

	json_sendRxData();
	json_sendFooter();
}

/*
   Handler for the API v2 set speed call
*/
void apiv2_setSpeed(void) {
	uint8_t	speed, timer;
	size_t		result;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the speed parameter is present */
	if (!server.hasArg("speed")) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'speed' is missing");
		return;
	} else {
		if (server.arg("speed") == "low")
			speed = FAN_SPEED_LOW;
		else if (server.arg("speed") == "medium")
			speed = FAN_SPEED_MEDIUM;
		else if (server.arg("speed") == "high")
			speed = FAN_SPEED_HIGH;
		else if (server.arg("speed") == "max")
			speed = FAN_SPEED_MAX;
		else {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Invalid value for parameter 'speed'");
			return;
		}
	}

	/* Check if the timer parameter is present */
	if (server.hasArg(MSG_API_TIMER)) {
		timer = server.arg(MSG_API_TIMER).toInt();
	} else {
		timer = 0;
	}

	/* Send the command to the fan unit */
	result = fan->setSpeed(speed, timer);
	switch (result) {
		case FAN_RESULT_SUCCESS :
			json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
			Serial.printf("Debug: setspeed ok\n");
			break;

		default :
			json_sendHeader(HTTP_CODE_OK, MSG_API_NO_REPLY, "Command sent but no acknowledge was received");
			Serial.printf("Debug: setspeed returned %i\n", result);
			break;
	}

	json_sendRxData();
	json_sendFooter();
}

