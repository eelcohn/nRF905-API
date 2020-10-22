#include "json_api.h"						// 
#include "../hardware/nrf905.h"				// NRF905_REGISTER_COUNT, nrf905->*
#include "../html/html.h"					// HTTP_CODE_*
#include "../language/language.h"				// MSG_API_*
#include "../../nRF905API.h"					// rxbuffersize
#include "../../board.h"					// PROGMEM
#include "../../utils.h"					// binToHexString()

extern NVRAMBuffer			config;		// Defined in nRF905API.ino
extern uint8_t				rxbuffer[][NRF905_MAX_FRAMESIZE];	// Defined in nRF905API.ino
extern bool				rx_buffer_overflow;	// Defined in nRF905API.ino
extern size_t				rxnum;			// Defined in nRF905API.ino

void json_sendHeader(const int httpCode, const char *result, const char *errormsg) {
	Serial.printf("http: %s\n", server.uri().c_str());

	server.setContentLength(CONTENT_LENGTH_UNKNOWN);	// Start chunked HTTP transfer
	server.send(httpCode, HTTP_JSON_RESPONSE, "");
	server.sendContent("{\"result\":\"");
	server.sendContent(result);
	server.sendContent("\"");
	if (strlen(errormsg) != 0) {
		server.sendContent(",\"error\":\"" + String(errormsg) + "\"");
	}
}

void json_sendFooter(void) {
	server.sendContent("}");
	server.sendContent("");				// Finish chunked HTTP transfer
}

void json_sendRxData(void) {
	char		rxhexstring[(NRF905_MAX_FRAMESIZE * 2) + 1] = {0};
	size_t		i;

	server.sendContent(",\"rxdata\":[");
	for (i = 0; i < rxnum; i++) {
		if (i != 0)
			server.sendContent(",");
		binToHexstring(rxbuffer[i], rxhexstring, nrf905->getRxPayloadWidth());
		server.sendContent("\"" + String(rxhexstring) + "\"");
//		JsonObject rxdata_0 = rxdata.createNestedObject();
//		rxdata_0["timestamp"] = "na";
//		rxdata_0["data"] = rxhexstring;
//		rxdata_0["rx_type"] =  rxbuffer[i][0x00];
//		rxdata_0["rx_id"] =  rxbuffer[i][0x01];
//		rxdata_0["tx_type"] =  rxbuffer[i][0x02];
//		rxdata_0["tx_id"] =  rxbuffer[i][0x03];
//		rxdata_0["ttl"] =  rxbuffer[i][0x04];
//		rxdata_0["command"] =  rxbuffer[i][0x05];
//		rxdata_0["parameters"] =  rxbuffer[i][0x06];
//		rxdata_0["param1"] =  rxbuffer[i][0x07];
//		rxdata_0["param2"] =  rxbuffer[i][0x08];
//		rxdata_0["param3"] =  rxbuffer[i][0x09];
//		rxdata_0["param4"] =  rxbuffer[i][0x0A];
//		rxdata_0["param5"] =  rxbuffer[i][0x0B];
//		rxdata_0["param6"] =  rxbuffer[i][0x0C];
//		rxdata_0["param7"] =  rxbuffer[i][0x0D];
//		rxdata_0["param8"] =  rxbuffer[i][0x0E];
//		rxdata_0["param9"] =  rxbuffer[i][0x0F];
//		rxdata.add(rxdata_0);
		memset(rxbuffer[i], 0, sizeof(rxbuffer[0]));				// TODO not sure if this is nessecary, but we'll clear the rxbuffer just in case
	}
	server.sendContent("]");
	rxnum = 0;
	rx_buffer_overflow = false;
}

/*
   Send an error API reply
*/
void json_sendReply(const int httpCode, const char *result, const char *errormsg) {
	json_sendHeader(httpCode, result, errormsg);
	json_sendFooter();
}

void json_sendDeprecated() {
	if (!server.authenticate(config.http_username, config.http_password)) {
		return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
	}

	json_sendReply(HTTP_CODE_NOT_FOUND, "not_found", MSG_API_DEPRECATED);
}

/* Handler for 404: Not found */
void json_sendNotFound() {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, String(FPSTR(HTTP_HTML_AUTH_FAILED)));
		}
	}

	json_sendReply(HTTP_CODE_NOT_FOUND, MSG_API_NOT_FOUND, "Endpoint not found");
}

