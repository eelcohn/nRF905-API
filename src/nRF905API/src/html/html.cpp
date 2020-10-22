#include "html.h"
#include "configwizard.h"					// HTML_CONFIG_LANGUAGE_URL
#include "icons.h"
#include "../../config.h"
#include "../../fan.h"						// Fan class, profile_buva, profile_zehnder, profile_default, network_link_id
#include "../../nvram.h"					// NVRAMBuffer
#include "../../utils.h"					// uint8ToString(), uint32ToString(), wifiEncryptionType()
#include "../language/language.h"				// MSG_EN_*
#include "../hardware/nrf905.h"				// nrf905->*

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config.*: Defined in nRF905API.ino
extern Fan *				fan;			// fan->*: Defined in nRF905API.ino

void htmlAddHandlers(void) {
	server.on(FPSTR(HTML_ROOT_URL)		, htmlRoot);
	server.on(FPSTR(HTML_CONFIG_URL)	, htmlConfig);
	server.on(FPSTR(HTML_FAN_URL)		, htmlFan);
}

/*
   Handler for root: redirect to configuration wizard
*/
void htmlRoot(void) {
	server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_LANGUAGE_URL));
	server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
	server.send(HTTP_CODE_SEE_OTHER);
}

/* Handler for Wifi AP configuration page */
void htmlConfig() {
	size_t		i,
			networksFound;

	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	networksFound = WiFi.scanNetworks();	// TODO move this to board->

	htmlHeader();
	if (server.hasArg("submit")) {
		/* TODO input validation */
		config.signature = NVRAM_SIGNATURE;
		config.version = NVRAM_VERSION;
		strncpy(config.wifi_hostname, server.arg("hostname").c_str(), sizeof(MAX_WIFI_HOSTNAME));
		strncpy(config.wifi_ssid, server.arg("ssid").c_str(), sizeof(config.wifi_ssid));
		strncpy(config.wifi_password, server.arg("wifi_password").c_str(), sizeof(config.wifi_password));
		if (server.hasArg("ntpserver"))
			strncpy(config.ntp_server, server.arg("ntpserver").c_str(), sizeof(MAX_NTPSERVER));
		else
			strncpy(config.ntp_server, DEFAULT_NTP_SERVER, sizeof(MAX_NTPSERVER));
		if (server.hasArg("ntpoffset"))
			config.ntp_offset = server.arg("ntpoffset").toInt();
		else
			config.ntp_offset = DEFAULT_NTP_OFFSET;
		if (server.hasArg("ntpinterval"))
			config.ntp_interval = server.arg("ntpinterval").toInt();
		else
			config.ntp_interval = DEFAULT_NTP_INTERVAL;
		if (server.hasArg("ota_password"))
			strncpy(config.ota_password, server.arg("ota_password").c_str(), sizeof(MAX_OTA_PASSWORD));
		else
			strncpy(config.ota_password, DEFAULT_OTA_PASSWORD, sizeof(MAX_OTA_PASSWORD));
		if (server.hasArg("ota_port"))
			config.ota_port = server.arg("ota_port").toInt();
		else
			config.ota_port = DEFAULT_OTA_PORT;
		strncpy(config.http_username, server.arg("http_username").c_str(), sizeof(MAX_HTTP_USERNAME));
		strncpy(config.http_password, server.arg("http_password").c_str(), sizeof(MAX_HTTP_PASSWORD));

		switch (server.arg("profile").toInt()) {
			case FAN_PROFILE_ZEHNDER :	// Zehnder
				nrf905->restoreConfigRegisters(fan_profiles[FAN_PROFILE_ZEHNDER]);
				nrf905->decodeConfigRegisters();
				nrf905->setRxAddress(network_link_id);
				nrf905->setTxAddress(network_link_id);
				break;

			case FAN_PROFILE_BUVA :	// BUVA
				nrf905->restoreConfigRegisters(fan_profiles[FAN_PROFILE_BUVA]);
				nrf905->decodeConfigRegisters();
				nrf905->setRxAddress(network_link_id);
				nrf905->setTxAddress(network_link_id);
				break;

			case FAN_PROFILE_CUSTOM :	// Custom
				// TODO 
				break;

			case FAN_PROFILE_DEFAULT :	// Default
			default :
				nrf905->restoreConfigRegisters(fan_profiles[FAN_PROFILE_DEFAULT]);
				nrf905->decodeConfigRegisters();
				nrf905->setTxAddress(network_default_id);
				break;
		}

		nrf905->setModeIdle();
		nrf905->encodeConfigRegisters();
		nrf905->writeConfigRegisters();
		nrf905->writeTxAddress();
		config.fan_my_device_type = 0x00;
		config.fan_my_device_id = 0x00;
		config.fan_main_unit_type = 0x00;
		config.fan_main_unit_id = 0x00;
		writeNVRAM();
		board->CommitNVRAM();
		server.sendContent("	<div class=\"message\">WiFi is now configured for SSID " + String(config.wifi_ssid) + ". Rebooting...</div>\n");
		htmlFooter();

		Serial.printf("WiFi SSID set to %s\n", config.wifi_ssid);

		delay(5000);
		board->reset();
	}
	server.sendContent(	"	<form action=\"" + server.uri() + "\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_SETTINGS_URL "\"></div>\n" \
				"		<fieldset><legend><script>w('lng')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"language\"><script>w('lng')</script></label></div><div class=\"form-input\"><select id=\"language\" name=\"language\">");
	for (i = 0; i < sizeof(LANGUAGES) / sizeof(LANGUAGES)[0]; i++) {
		server.sendContent("<option value=\"" + String(FPSTR(LANGUAGE_CODES[i])) + "\"");
		if (strcmp_P(config.language, (const char *) &LANGUAGE_CODES[i]) == 0)
			server.sendContent(F(" selected=\"selected\""));
		server.sendContent(">" + String(FPSTR(LANGUAGES[i])) + "</option>");
	}
	server.sendContent(	F("</select></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<fieldset><legend><script>w('aut')</script></legend>\n"));
	server.sendContent(	"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"http_username\"><script>w('unm')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"http_username\" maxlength=\"" + String(sizeof(config.http_username) - 1) + "\" name=\"http_username\" value=\"" + String(config.http_username) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"http_password\"><script>w('pwd')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"http_password\" maxlength=\"" + String(sizeof(config.http_password) - 1) + "\" name=\"http_password\" value=\"" + String(config.http_password) + "\"></div><div class=\"form-icon\"><img alt=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_http_password\" onclick=\"changeInputType('http_password', 'icon_http_password');\"></div></div>\n");
	server.sendContent(	F("		</fieldset>\n" \
				"		<fieldset><legend><script>w('wfi')</script></legend>\n"));
	server.sendContent(	"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"hostname\"><script>w('hnm')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"hostname\" maxlength=\"" + String(sizeof(config.wifi_hostname) - 1) + "\" name=\"hostname\" value=\"" + String(config.wifi_hostname) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ssid\"><script>w('sid')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"ssid\" list=\"ssids\" maxlength=\"" + String(sizeof(config.wifi_ssid) - 1) + "\" name=\"ssid\"><datalist id=\"ssids\">");
	for (i = 0; i < networksFound; i++)
		server.sendContent("<option value=\"" + String(WiFi.SSID(i)) + "\">" + String(WiFi.RSSI(i)) + "dBm</option>");	// TODO move this to board->
	server.sendContent(	F("</datalist></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"wifi_password\"><script>w('pwd')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"wifi_password\" maxlength=\"64\" name=\"wifi_password\"></div><div class=\"form-icon\"><img alt=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_wifi_password\" onclick=\"changeInputType('wifi_password', 'icon_wifi_password');\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<fieldset><legend><script>w('n95')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"profile\">" MSG_EN_PROFILE "</label></div><div class=\"form-input\"><select id=\"profile\" name=\"profile\"><option value=\"1\">" MSG_EN_ZEHNDER "</option><option value=\"2\">" MSG_EN_BUVA "</option><option value=\"3\">" MSG_EN_CUSTOM "</option><option value=\"4\">" MSG_EN_DEFAULT "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n"));
#if NRF905API_NTP == 1
	server.sendContent(	"		<fieldset><legend><script>w('ntp')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpserver\"><script>w('nts')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"ntpserver\" maxlength=\"" + String(sizeof(config.ntp_server) - 1) + "\" name=\"ntpserver\" value=\"" + String(config.ntp_server) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpoffset\"><script>w('nto')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ntpoffset\" maxlength=\"" + String(sizeof(config.ntp_offset)) + "\" name=\"ntpoffset\" value=\"" + String(config.ntp_offset) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpinterval\"><script>w('nti')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ntpinterval\" maxlength=\"" + String(sizeof(config.ntp_interval)) + "\" name=\"ntpinterval\" value=\"" + String(config.ntp_interval) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n");
#endif
#if NRF905API_OTA == 1
	server.sendContent(	"		<fieldset><legend><script>w('ous')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ota_password\"><script>w('phs')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"ota_password\" maxlength=\"" + String(sizeof(config.ota_password) - 1) + "\" name=\"ota_password\" value=\"" + String(config.ota_password) + "\"></div><div class=\"form-icon\"><img alt=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_ota_password\" onclick=\"changeInputType('ota_password', 'icon_ota_password');\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ota_port\"><script>w('prt')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ota_port\"  name=\"ota_port\" min=\"1\" max=\"65535\" value=\"" + String(config.ota_port) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n");
#endif
	server.sendContent(	F("		<div class=\"form-row\"><input type=\"submit\" id=\"submit\" name=\"submit\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

void htmlFan(void) {
	uint8_t	speed,
			timer;

	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	htmlHeader();
	server.sendContent(	F("	<form action=\"/fan/config.html\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_FAN_URL "\"></div>\n"));
	if (server.hasArg("submit")) {
		speed = server.arg("speed").toInt();
		timer = server.arg("timer").toInt();
		if (fan->setSpeed(speed, timer) == true) {
			server.sendContent(F("	<div class=\"message\"><script>w('fss')</script></div>\n"));
		} else {
			server.sendContent(F("	<div class=\"message\"><script>w('fnr')</script</div>\n"));
		}
	}
	server.sendContent(	F("		<fieldset><legend>" MSG_EN_FAN_SETTING "</legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"low\">" MSG_EN_LOW "</label></div><div class=\"form-input\"><input type=\"radio\" id=\"low\" name=\"speed\" value=\"1\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"medium\">" MSG_EN_MEDIUM "</label></div><div class=\"form-input\"><input type=\"radio\" id=\"medium\" name=\"speed\" value=\"2\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"high\">" MSG_EN_HIGH "</label></div><div class=\"form-input\"><input type=\"radio\" id=\"high\" name=\"speed\" value=\"3\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"max\">" MSG_EN_MAX "</label></div><div class=\"form-input\"><input type=\"radio\" id=\"max\" name=\"speed\" value=\"4\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"timer\">" MSG_EN_DURATION "</label></div><div class=\"form-input\"><input type=\"number\" id=\"timer\" name=\"timer\" min=\"0\" max=\"255\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><input type=\"submit\" name=\"speed\" value=\"" MSG_EN_LOW "\"><input type=\"submit\" name=\"speed\" value=\"" MSG_EN_MEDIUM "\"><input type=\"submit\" name=\"speed\" value=\"" MSG_EN_HIGH "\"><input type=\"submit\" name=\"speed\" value=\"" MSG_EN_MAX "\"></div>\n" \
				"		</fieldset>\n" \
				"		<div class=\"form-row\"><input type=\"submit\" id=\"submit\" name=\"submit\" value=\">\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

void htmlHeader(void) {
	Serial.println("http: " + server.uri());

	server.setContentLength(CONTENT_LENGTH_UNKNOWN);	// Start chunked HTTP transfer
	server.send(HTTP_CODE_OK, HTTP_HTML_RESPONSE, "");
	server.sendContent(FPSTR(HTML_HEADER1));
	server.sendContent(SELECTED_LANGUAGE);
	server.sendContent(FPSTR(HTML_HEADER2));
	server.sendContent(MSG_EN_TITLE);
	server.sendContent(FPSTR(HTML_HEADER3));
}

void htmlFooter(void) {
	server.sendContent_P(HTML_FOOTER);
	server.sendContent("");				// Finish chunked HTTP transfer
}

