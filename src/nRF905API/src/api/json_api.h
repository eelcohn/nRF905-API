#ifndef __HANDLER_JSONAPI_H__
#define __HANDLER_JSONAPI_H__

#include "json_api_v1.h"
#include "../../config.h"			// NRF905API_FAN
#if NRF905API_FAN == 1
#include "json_api_v2_fan.h"
#include "json_api_test.h"
#endif

const char HTTP_JSON_RESPONSE[] PROGMEM	= "application/json; charset=utf-8;";

/* HTML header and footer */
const char JSON_HEADER[] PROGMEM		= \
	"{\n" \
	"	\"result\":\"";
const char JSON_FOOTER[] PROGMEM		= \
	"\n}";

void json_sendHeader(const int httpCode, const char *result, const char *errormsg);
void json_sendFooter(void);
void json_sendRxData(void);
void json_sendReply(const int httpCode, const char *result, const char *errormsg);
void json_sendDeprecated(void);
void json_sendNotFound();

#endif

