#ifndef __HANDLER_JSONAPIV2_FAN_H__
#define __HANDLER_JSONAPIV2_FAN_H__

#if NRF905API_FAN == 1 

#include "../../board.h"		// PROGMEM

const char API_V2_FAN_CONFIG_URL[] PROGMEM		= "/api/v2/fan/config.json";
const char API_V2_FAN_LINK_URL[] PROGMEM		= "/api/v2/fan/link.json";
const char API_V2_FAN_SETSPEED_URL[] PROGMEM		= "/api/v2/fan/setspeed.json";
const char API_V2_FAN_SETVOLTAGE_URL[] PROGMEM		= "/api/v2/fan/setvoltage.json";

void apiv2_AddHandlers(void);
void apiv2_config(void);
void apiv2_discover(void);
void apiv2_setVoltage(void);
void apiv2_setSpeed(void);
#endif

#endif

