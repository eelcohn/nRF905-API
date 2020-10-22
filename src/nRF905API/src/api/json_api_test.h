#ifndef __JSON_API_TEST_H__
#define __JSON_API_TEST_H__

#include "../../board.h"		// PROGMEM
#include "../../config.h"					// NRF905API_FAN

#if NRF905API_FAN == 1

const char API_V2_FAN_QUERYDEVICE_URL[] PROGMEM	= "/api/test/fan/querydevice.json";
const char API_V2_FAN_QUERYNETWORK_URL[] PROGMEM	= "/api/test/fan/querynetwork.json";
const char API_V2_FAN_NETSCAN[] PROGMEM		= "/api/test/fan/netscan.json";
const char API_V2_FAN_DEVSCAN_URL[] PROGMEM		= "/api/test/fan/devscan.json";
const char API_V2_FAN_PORTSCAN_URL[] PROGMEM		= "/api/test/fan/portscan.json";
const char API_V2_FAN_RETRANSMIT_URL[] PROGMEM	= "/api/test/fan/retransmit.json";

void apitest_AddHandlers(void);
void apiv2_queryDevice(void);
void apiv2_queryNetwork(void);
void apiv2_networkScan(void);
void apiv2_deviceScan(void);
void apiv2_portScan(void);
void apiv2_retransmit(void);

#endif

#endif

