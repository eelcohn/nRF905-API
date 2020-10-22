#ifndef __HANDLER_JSONAPIV1_H__
#define __HANDLER_JSONAPIV1_H__

#include "../../board.h"				// PROGMEM

const char API_V1_CONFIG_URL[] PROGMEM			= "/api/v1/config.json";
const char API_V1_RECEIVE_URL[] PROGMEM		= "/api/v1/receive.json";
const char API_V1_SEND_URL[] PROGMEM			= "/api/v1/send.json";
const char API_V1_STATUS_URL[] PROGMEM			= "/api/v1/status.json";
const char API_V1_SYSTEMCONFIG_URL[] PROGMEM		= "/api/v1/systemconfig.json";
const char API_V1_ZEHNDER_SETPOWER_URL[] PROGMEM	= "/api/v1/zehnder/setpower.json";
const char API_V1_ZEHNDER_SETTIMER_URL[] PROGMEM	= "/api/v1/zehnder/settimer.json";
const char API_V1_ZEHNDER_LINK_URL[] PROGMEM		= "/api/v1/zehnder/link.json";
const char API_V1_ZEHNDER_CONFIG_URL[] PROGMEM		= "/api/v1/zehnder/config.json";

void apiv1_AddHandlers(void);
void apiv1_config(void);
void apiv1_receive(void);
void apiv1_send(void);
void apiv1_status(void);
void apiv1_systemConfig(void);

#endif

