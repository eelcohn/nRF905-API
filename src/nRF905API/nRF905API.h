/*
  nRF905 Arduino API - API to control a nRF905 transceiver module
  written by Eelco Huininga 2020
*/

#ifndef __NRF905API_H__
#define __NRF905API_H__

#if NRF905API_HTTPS == 1
#include "certificate.h"
#endif

#define FIRMWARE_TITLE			"nRF905 API"
#define FIRMWARE_VERSION		"1.0.0"

#define MAX_RXBUFFER_SIZE		100				// A maximum of 100 received frames can be cached TODO: use dynamic memory allocation to allow usage of max available free RAM for storing rxdata
#define MAX_WIFI_CONNECT_RETRIES	60				// Try to connect to WiFi for a maximum of 30 seconds
#define MAX_WIFI_AP_ONLINE_TIME	600000				// Switch off AP when in AP mode to prevent misuse

const char firmware_title[] PROGMEM	= FIRMWARE_TITLE;
const char firmware_version[] PROGMEM	= FIRMWARE_VERSION;
const size_t rxbuffersize PROGMEM	= MAX_RXBUFFER_SIZE;

void IRAM_ATTR rxISR(void);

#endif

