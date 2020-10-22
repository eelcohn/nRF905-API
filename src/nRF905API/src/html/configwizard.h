#ifndef __CONFIGWIZARD_H__
#define __CONFIGWIZARD_H__

#include "../../board.h"				// PROGMEM

const char HTML_CONFIG_LANGUAGE_URL[] PROGMEM		= "/cfglang";
const char HTML_CONFIG_AUTH_URL[] PROGMEM		= "/cfgauth";
const char HTML_CONFIG_WIFI_URL[] PROGMEM		= "/cfgwifi";
const char HTML_CONFIG_ADVANCED_URL[] PROGMEM		= "/cfgadv";
const char HTML_CONFIG_FAN_URL[] PROGMEM		= "/cfgfan";
const char HTML_CONFIG_FAN_DISCOVER_URL[] PROGMEM	= "/cfgfand";
const char HTML_CONFIG_FAN_MANUAL_URL[] PROGMEM	= "/cfgfanm";
const char HTML_CONFIG_REBOOT_URL[] PROGMEM		= "/cfgboot";

void htmlConfigWizardAddHandlers(void);
void htmlConfigWizardLanguage(void);
void htmlConfigWizardAuthentication(void);
void htmlConfigWizardWifi(void);
void htmlConfigWizardAdvanced(void);
void htmlConfigWizardFan(void);
void htmlConfigWizardFanDiscover(void);
void htmlConfigWizardFanManual(void);
void htmlConfigWizardReboot(void);

#endif

