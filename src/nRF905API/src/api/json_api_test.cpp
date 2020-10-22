#include "json_api.h"						// json_sendReply(), HTTP_JSON_RESPONSE
#include "json_api_test.h"					// API_V2_FAN_*_URL
#include "../../config.h"					// NRF905API_FAN
#include "../../fan.h"						// Fan class, FAN_FREQUENCY_ZEHNDER, FAN_FREQUENCY_BUVA
#include "../../utils.h"					// isHex()
#include "../hardware/nrf905.h"				// nrf905->*
#include "../html/html.h"					// HTTP_CODE_*
#include "../language/language.h"				// MSG_API_*

extern NVRAMBuffer			config;		// Defined in nRF905API.ino
extern Fan *				fan;			// Defined in nRF905API.ino
extern size_t				rxnum;			// Defined in nRF905API.ino

void apitest_AddHandlers(void) {
#if NRF905API_FAN == 1
	server.on(FPSTR(API_V2_FAN_QUERYDEVICE_URL),		HTTP_GET, apiv2_queryDevice);
	server.on(FPSTR(API_V2_FAN_NETSCAN),			HTTP_GET, apiv2_networkScan);
	server.on(FPSTR(API_V2_FAN_DEVSCAN_URL),		HTTP_GET, apiv2_deviceScan);
	server.on(FPSTR(API_V2_FAN_PORTSCAN_URL),		HTTP_GET, apiv2_portScan);
	server.on(FPSTR(API_V2_FAN_RETRANSMIT_URL),		HTTP_GET, apiv2_retransmit);
#endif
}

/*
   Handler for the API v2 query device settings call
*/
void apiv2_queryDevice(void) {
	char		tempstr[6];
	uint8_t	device_id,
			speed,
			flags,
			voltage;
	size_t		result;

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
		device_id = config.fan_main_unit_id;
	}

	/* Send the command to the fan unit */
	result = fan->queryDevice(device_id, &speed, &voltage, &flags);
	switch (result) {
		case FAN_RESULT_SUCCESS : {
			Serial.printf("querydevice: ok\n");
			json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
			server.sendContent(",\"devices\":{");
			uint8ToString(device_id, tempstr);
			server.sendContent("\"" + String(tempstr) + "\":{");
			server.sendContent("\"type\": \"fan\",");
			server.sendContent("\"id\": \"" + String(tempstr) + "\",");
			sprintf(tempstr, "%.1f", ((float)voltage / 10));
			server.sendContent("\"voltage\": " + String(tempstr) + ",");
			server.sendContent("\"percentage\": " + String(voltage) + ",");
			switch (speed) {
				case FAN_SPEED_AUTO :
					server.sendContent("\"speed\": \"auto\",");
					break;

				case FAN_SPEED_LOW :
					server.sendContent("\"speed\": \"low\",");
					break;

				case FAN_SPEED_MEDIUM :
					server.sendContent("\"speed\": \"medium\",");
					break;

				case FAN_SPEED_HIGH :
					server.sendContent("\"speed\": \"high\",");
					break;

				case FAN_SPEED_MAX :
					server.sendContent("\"speed\": \"max\",");
					break;

				default :
					uint8ToString(speed, tempstr);
					server.sendContent("\"speed\": \"" + String(tempstr) + "\",");
					break;
			}
			server.sendContent("\"timer\": " + String(((flags & 0x01) == 0x01) ? "true" : "false") + "}}");
			break;
		}

		default :
			Serial.printf("querydevice: returned %i\n", result);
			json_sendHeader(HTTP_CODE_OK, MSG_API_NO_REPLY, "Command sent but no acknowledge was received");
			break;
	}
	json_sendRxData();
	json_sendFooter();
}

/*
   Handler for the API v2 query device settings call
*/
void apiv2_queryNetwork(void) {
	// TODO send 0x0D or 0x10 frame
}

/* Handler for the API v2 network scan */
void apiv2_networkScan(void) {
	uint32_t	start,
			end;
	size_t		result;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the start parameter is present */
	if (!server.hasArg(MSG_API_START)) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'start' missing");
		return;
	} else {
		/* Check if the start parameter is valid */
		if (!isHex(server.arg(MSG_API_START).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'start' is not valid hex");
			return;
		}
		start = strtoul(server.arg(MSG_API_START).c_str(), 0, 16);
	}

	/* Check if the end parameter is present */
	if (!server.hasArg(MSG_API_END)) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'end' missing");
		return;
	} else {
		/* Check if the end parameter is valid */
		if (!isHex(server.arg(MSG_API_END).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'end' is not valid hex");
			return;
		}
		end = strtoul(server.arg(MSG_API_END).c_str(), 0, 16);
	}

	/* Start the network scan */
	Serial.printf("Network scan from %08X to %08X", start, end);
	result = fan->networkScan(start, end);
	Serial.printf(" done - found %i open networks.\n", result);

	/* Write HTTP result */
	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	server.sendContent(",\"networks_found\":" + String(result) + "\"");
	json_sendRxData();
	json_sendFooter();
}

/* Handler for the API v2 device scan */
void apiv2_deviceScan(void) {
	size_t		result;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	Serial.printf("Device scan on network %08X", config.nrf905_tx_address);

	result = fan->deviceScan(config.nrf905_tx_address);

	Serial.println("Done");

	/* Write HTTP result */
	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	server.sendContent(",\"devices_found\":" + String(result) + "\"");
	json_sendRxData();
	json_sendFooter();
}

/* Handler for the API v2 port (command) scan */
void apiv2_portScan(void) {
	uint8_t	device_type,
			device_id;
	size_t		result;

	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	/* Check if the device_type parameter is present */
	if (!server.hasArg(MSG_API_DEVICE_TYPE)) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_type' missing");
		return;
	} else {
		/* Check if the end parameter is valid */
		if (!isHex(server.arg(MSG_API_DEVICE_TYPE).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_type' is not valid hex");
			return;
		}
		device_type = strtoul(server.arg(MSG_API_DEVICE_TYPE).c_str(), 0, 16);
	}

	/* Check if the device_id parameter is present */
	if (!server.hasArg(MSG_API_DEVICE_ID)) {
		json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_id' missing");
		return;
	} else {
		/* Check if the end parameter is valid */
		if (!isHex(server.arg(MSG_API_DEVICE_ID).c_str())) {
			json_sendReply(HTTP_CODE_OK, MSG_API_ERROR, "Parameter 'device_id' is not valid hex");
			return;
		}
		device_id = strtoul(server.arg(MSG_API_DEVICE_ID).c_str(), 0, 16);
	}

	Serial.printf("Port scan on device type %02X device id %02X on network %02X", device_type, device_id, config.nrf905_tx_address);

	result = fan->portScan(config.nrf905_tx_address, device_type, device_id, 0x01, 0x01);

	Serial.println("Done");

	/* Write HTTP result */
	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	server.sendContent(",\"ports_found\":" + String(result) + "\"");
	json_sendRxData();
	json_sendFooter();
}

void apiv2_retransmit(void) {
	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	if (server.arg("retransmit") == MSG_API_TRUE) {
		fan->test_retransmit(true);
	}

	if (server.arg("retransmit") == MSG_API_FALSE) {
		fan->test_retransmit(false);
	}

	/* Write HTTP result */
	json_sendHeader(HTTP_CODE_OK, MSG_API_OK, "");
	json_sendRxData();
	json_sendFooter();
}

