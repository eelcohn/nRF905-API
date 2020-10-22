#ifndef __SSDP_H__
#define __SSDP_H__

#if NRF905API_SSDP == 1
const char SSDP_DESCRIPTION_URL[] PROGMEM	= "/description.xml";
const char SSDP_SCHEMA_URL[] PROGMEM		= "description.xml";
const char SSDP_MANUFACTURER_URL[] PROGMEM	= "https://github.com/eelcohn/nRF905-API/";
const char SSDP_MANUFACTURER[] PROGMEM		= "eelcohn";
#endif

#endif

