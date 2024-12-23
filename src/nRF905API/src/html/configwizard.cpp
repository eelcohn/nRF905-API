#include "configwizard.h"
#include "html.h"
#include "icons.h"
#include "javascript.h"
#include "../language/language.h"
#include "../../fan.h"						// Fan class
#include "../../utils.h"					// uint8ToString(), uint32ToString()

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config.*: Defined in nRF905API.ino
extern Fan *				fan;			// fan->*: Defined in nRF905API.ino

void htmlConfigWizardAddHandlers(void) {
	server.on(FPSTR(HTML_CONFIG_LANGUAGE_URL)		, htmlConfigWizardLanguage);
	server.on(FPSTR(HTML_CONFIG_AUTH_URL)			, htmlConfigWizardAuthentication);
	server.on(FPSTR(HTML_CONFIG_WIFI_URL)			, htmlConfigWizardWifi);
	server.on(FPSTR(HTML_CONFIG_ADVANCED_URL)		, htmlConfigWizardAdvanced);
	server.on(FPSTR(HTML_CONFIG_FAN_URL)			, htmlConfigWizardFan);
	server.on(FPSTR(HTML_CONFIG_FAN_DISCOVER_URL)		, htmlConfigWizardFanDiscover);
	server.on(FPSTR(HTML_CONFIG_FAN_MANUAL_URL)		, htmlConfigWizardFanManual);
	server.on(FPSTR(HTML_CONFIG_REBOOT_URL)		, htmlConfigWizardReboot);
}

/*
   Configuration wizard step 1 - Language
*/
void htmlConfigWizardLanguage(void) {
	size_t i;

	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.method() == HTTP_POST) {
		if (server.hasArg("language")) {
			strncpy(config.language, server.arg("language").c_str(), MAX_LANGUAGE);
		}
	}

	if (server.hasArg("next")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_AUTH_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfglang\" action=\"/cfglang\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_LANGUAGE_URL "\"></div>\n" \
				"		<fieldset><legend><script>w('rgs')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"language\"><script>w('lng')</script></label></div><div class=\"form-input\"><select id=\"language\" name=\"language\" onchange=\"this.form.submit();\">"));
	for (i = 0; i < sizeof(LANGUAGES) / sizeof(LANGUAGES)[0]; i++) {
		server.sendContent("<option value=\"" + String(FPSTR(LANGUAGE_CODES[i])) + "\"");
		if (strcmp_P(config.language, (const char *) &LANGUAGE_CODES[i]) == 0)
			server.sendContent(F(" selected=\"selected\""));
		server.sendContent(">" + String(FPSTR(LANGUAGES[i])) + "</option>");
	}
	server.sendContent(	F("</select></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<div class=\"form-row\"><input autofocus type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
   Configuration wizard step 2 - Authentication
*/
void htmlConfigWizardAuthentication(void) {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.method() == HTTP_POST) {
		if (server.hasArg("http_username")) {
			strncpy(config.http_username, server.arg("http_username").c_str(), MAX_HTTP_USERNAME);
		}
		if (server.hasArg("http_password")) {
			strncpy(config.http_password, server.arg("http_password").c_str(), MAX_HTTP_PASSWORD);
		}
	}

	if (server.hasArg("next")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_WIFI_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgauth\" action=\"/cfgauth\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"/i/sec.svg\"></div>\n" \
				"		<fieldset><legend><script>w('aut')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"http_username\"><script>w('unm')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"http_username\" maxlength=\"32\" name=\"http_username\" value=\"" DEFAULT_HTTP_USERNAME "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"http_password\"><script>w('pwd')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"http_password\" maxlength=\"32\" name=\"http_password\" value=\"" DEFAULT_HTTP_PASSWORD "\"></div><div class=\"form-icon\"><img alt=\"\" title=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_http_password\" onclick=\"changeInputType('http_password', 'icon_http_password');\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<div class=\"form-row\"><input autofocus type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
   Configuration wizard step 3 - WiFi settings
*/
void htmlConfigWizardWifi(void) {
	size_t	i,
		networksFound;

	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.method() == HTTP_POST) {
		if (server.hasArg("hostname")) {
			strncpy(config.wifi_hostname, server.arg("hostname").c_str(), sizeof(config.wifi_hostname));
		}
		if (server.hasArg("ssid")) {
			strncpy(config.wifi_ssid, server.arg("ssid").c_str(), sizeof(config.wifi_ssid));
		}
		if (server.hasArg("wifi_password")) {
			strncpy(config.wifi_password, server.arg("wifi_password").c_str(), sizeof(config.wifi_password));
		}
	}

	if (server.hasArg("next")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_FAN_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	networksFound = WiFi.scanNetworks();		// TODO move this to board->

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgwifi\" action=\"/cfgwifi\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_WIFI_URL "\"></div>\n" \
				"		<fieldset><legend><script>w('wfi')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"hostname\">" MSG_EN_HOSTNAME "</label></div><div class=\"form-input\"><input autofocus type=\"text\" id=\"hostname\" maxlength=\"32\" name=\"hostname\" autocomplete=\"new-password\" value=\"" DEFAULT_HOSTNAME "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ssid\">" MSG_EN_SSID "</label></div><div class=\"form-input\"><input type=\"text\" id=\"ssid\" list=\"ssids\" maxlength=\"32\" name=\"ssid\" autocomplete=\"new-password\"><datalist id=\"ssids\">"));
	for (i = 0; i < networksFound; i++)
		server.sendContent("<option value=\"" + String(WiFi.SSID(i)) + "\"></option>");	// TODO move this to board-> and add encryption type
//		server.sendContent("<option value=\"" + String(WiFi.SSID(i)) + "\">" + String(WiFi.RSSI(i)) + "dBm</option>");
	server.sendContent(	F("</datalist></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"wifi_password\"><script>w('pwd')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"wifi_password\" maxlength=\"64\" name=\"wifi_password\" autocomplete=\"new-password\"></div><div class=\"form-icon\"><img alt=\"\" title=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_wifi_password\" onclick=\"changeInputType('wifi_password', 'icon_wifi_password');\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<div class=\"form-row\"><input type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
  Configuration wizard step 4 - Fan settings
*/
void htmlConfigWizardFan(void) {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.method() == HTTP_POST) {
		if (server.hasArg("profile")) {
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
			config.fan_network_id = network_link_id;
		}
	}

	if (server.hasArg("next")) {
		switch (server.arg("fanconfig").toInt()) {
			case 1 :
				server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_FAN_DISCOVER_URL));
				break;

			case 2 :
				server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_FAN_MANUAL_URL));
				break;

			case 3 :
				server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_REBOOT_URL));
				break;

			default :
				server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_FAN_URL));
				break;
		}
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgfan\" action=\"/cfgfan\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_FAN_URL "\"></div>\n"));
	if (nrf905->testSPI() == false) {
		server.sendContent(F("		<div class=\"message\"><script>w('ncf')</script></div>\n"));
	}
	server.sendContent(	F("		<fieldset><legend>" MSG_EN_NRF905_CONFIGURATION "</legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lprofile\" for=\"profile\">" MSG_EN_PROFILE "</label></div><div class=\"form-input\"><select id=\"profile\" name=\"profile\" onchange=\"updateFanForm()\"><option value=\"1\" selected=\"selected\">" MSG_EN_ZEHNDER "</option><option value=\"2\">" MSG_EN_BUVA "</option><option value=\"3\">" MSG_EN_CUSTOM "</option><option value=\"4\">" MSG_EN_DEFAULT "</option></select></div><div class=\"form-icon\"></div></div>\n"));
	server.sendContent(	F("			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lfrequency\" for=\"frequency\" style=\"display:none;\">" MSG_EN_FREQUENCY "</label></div><div class=\"form-input\"><input type=\"text\" id=\"frequency\" minlength=\"9\" maxlength=\"9\" name=\"frequency\" value=\"433200000\" style=\"display:none;\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"ltxpower\" for=\"txpower\" style=\"display:none;\">" MSG_EN_TX_POWER "</label></div><div class=\"form-input\"><select id=\"txpower\" name=\"txpower\" style=\"display:none;\"><option value=\"-10\" selected=\"selected\">" MSG_EN_M10DBM "</option><option value=\"-2\">" MSG_EN_M2DBM "</option><option value=\"6\">" MSG_EN_P6DBM "</option><option value=\"10\">" MSG_EN_P10DBM "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lrxpower\" for=\"rxpower\" style=\"display:none;\">" MSG_EN_RX_POWER "</label></div><div class=\"form-input\"><select id=\"rxpower\" name=\"rxpower\" style=\"display:none;\"><option value=\"0\" selected=\"selected\">" MSG_EN_NORMAL "</option><option value=\"1\">" MSG_EN_REDUCED "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lretransmit\" for=\"retransmit\" style=\"display:none;\">" MSG_EN_AUTO_RETRANSMIT "</label></div><div class=\"form-input\"><select id=\"retransmit\" name=\"retransmit\" style=\"display:none;\"><option value=\"no\" selected=\"selected\">" MSG_EN_NO "</option><option value=\"yes\">" MSG_EN_YES "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"ltxaddrwidth\" for=\"txaddrwidth\" style=\"display:none;\">" MSG_EN_TX_ADDRESS_WIDTH "</label></div><div class=\"form-input\"><select id=\"txaddrwidth\" name=\"txaddrwidth\" style=\"display:none;\"><option value=\"1\">" MSG_EN_1_BYTE "</option><option value=\"4\" selected=\"selected\">" MSG_EN_4_BYTES "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lrxaddrwidth\" for=\"rxaddrwidth\" style=\"display:none;\">" MSG_EN_RX_ADDRESS_WIDTH "</label></div><div class=\"form-input\"><select id=\"rxaddrwidth\" name=\"rxaddrwidth\" style=\"display:none;\"><option value=\"1\">" MSG_EN_1_BYTE "</option><option value=\"4\" selected=\"selected\">" MSG_EN_4_BYTES "</option></select></div><div class=\"form-icon\"></div></div>\n"));
	server.sendContent(	F(	"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"ltxpayloadwidth\" for=\"txpayloadwidth\" style=\"display:none;\">" MSG_EN_TX_PAYLOAD_WIDTH "</label></div><div class=\"form-input\"><input type=\"number\" id=\"txpayloadwidth\" min=\"1\" max=\"32\" name=\"txpayloadwidth\" value=\"32\" style=\"display:none;\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lrxpayloadwidth\" for=\"rxpayloadwidth\" style=\"display:none;\">" MSG_EN_RX_PAYLOAD_WIDTH "</label></div><div class=\"form-input\"><input type=\"number\" id=\"rxpayloadwidth\" min=\"1\" max=\"32\" name=\"rxpayloadwidth\" value=\"32\" style=\"display:none;\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lrxaddress\" for=\"rxaddress\" style=\"display:none;\">" MSG_EN_RX_ADDRESS "</label></div><div class=\"form-input\"><input type=\"text\" id=\"rxaddress\" minlength=\"8\" maxlength=\"8\" name=\"rxaddress\" value=\"E7E7E7E7\" style=\"display:none;\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lupclk\" for=\"upclk\" style=\"display:none;\">" MSG_EN_OUTPUT_CLK_FREQUENCY "</label></div><div class=\"form-input\"><select id=\"upclk\" name=\"upclk\" style=\"display:none;\"><option value=\"4000000\">" MSG_EN_4MHZ "</option><option value=\"2000000\">" MSG_EN_2MHZ "</option><option value=\"1000000\">" MSG_EN_1MHZ "</option><option value=\"500000\" selected=\"selected\">" MSG_EN_500KHZ "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lupclken\" for=\"upclken\" style=\"display:none;\">" MSG_EN_OUTPUT_CLK_ENABLE "</label></div><div class=\"form-input\"><select id=\"upclken\" name=\"upclken\" style=\"display:none;\"><option value=\"no\" selected=\"selected\">" MSG_EN_NO "</option><option value=\"yes\">" MSG_EN_YES "</option></select></div><div class=\"form-icon\"></div></div>\n"));
	server.sendContent(	F("			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lxof\" for=\"xof\" style=\"display:none;\">" MSG_EN_XTAL_CLK_FREQUENCY "</label></div><div class=\"form-input\"><select id=\"xof\" name=\"xof\" style=\"display:none;\"><option value=\"4000000\">" MSG_EN_4MHZ "</option><option value=\"8000000\">" MSG_EN_8MHZ "</option><option value=\"12000000\">" MSG_EN_12MHZ "</option><option value=\"16000000\">" MSG_EN_16MHZ "</option><option value=\"20000000\" selected=\"selected\">" MSG_EN_20MHZ "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lcrcenable\" for=\"crcenable\" style=\"display:none;\">" MSG_EN_CRC_ENABLE "</label></div><div class=\"form-input\"><select id=\"crcenable\" name=\"crcenable\" style=\"display:none;\"><option value=\"no\">" MSG_EN_NO "</option><option value=\"yes\" selected=\"selected\">" MSG_EN_YES "</option></select></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label id=\"lcrcmode\" for=\"crcmode\" style=\"display:none;\">" MSG_EN_CRC_MODE "</label></div><div class=\"form-input\"><select id=\"crcmode\" name=\"crcmode\" style=\"display:none;\"><option value=\"8\">" MSG_EN_8_BITS "</option><option value=\"yes\" selected=\"selected\">" MSG_EN_16_BITS "</option></select></div><div class=\"form-icon\"></div></div>\n"));
	server.sendContent(	F("		</fieldset>\n" \
				"		<fieldset><legend><script>w('fpm')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"fanauto\"><script>w('fac')</script></label></div><div class=\"form-input\"><input autofocus type=\"radio\" id=\"fanauto\" name=\"fanconfig\" value=\"1\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"fanmanual\"><script>w('fmc')</script></label></div><div class=\"form-input\"><input type=\"radio\" id=\"fanmanual\" name=\"fanconfig\" value=\"2\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"fanskip\"><script>w('fsk')</script></label></div><div class=\"form-input\"><input type=\"radio\" id=\"fanskip\" name=\"fanconfig\" value=\"3\"></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n" \
				"		<div class=\"form-row\"><input type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
  Configuration wizard step 5 - Fan discovery
*/
void htmlConfigWizardFanDiscover(void) {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.hasArg("finish")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_REBOOT_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgfand\" action=\"/cfgfand\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n"));
	if (server.hasArg("plugin")) {
		Serial.printf_P("Fan discover step 2: plugin\n");
		server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_POWER_URL "\"></div>\n" \
					"		<div class=\"message\"><script>w('fpi')</script></div>\n" \
					"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"next\" name=\"discover\" value=\"\"></div>\n" \
					"		</div>\n" \
					"	</form>\n"));
	}
	else if (server.hasArg("discover")) {
		Serial.printf_P("Fan discover step 3: link\n");

		config.fan_my_device_type = FAN_TYPE_REMOTE_CONTROL;	// Set device type to remote control
		config.fan_my_device_id = fan->createDeviceID();		// Create a new device ID

		/* Start fan discovery */
		switch (fan->discover(config.fan_my_device_id, FAN_JOIN_DEFAULT_TIMEOUT)) {
			case FAN_RESULT_SUCCESS :
				Serial.printf_P("Fan configuration set: netid=%08X myid=%02X mainid=%02X\n", config.nrf905_tx_address, config.fan_my_device_id, config.fan_main_unit_id);
				server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_CHECK_URL "\"></div>\n" \
							"<div class=\"message\"><script>w('nnc')</script></div>"));
				break;

			case FAN_ERROR_NOT_FOUND :
				server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_WARNING_URL "\"></div>\n" \
							"<div class=\"message\"><script>w('nnd')</script></div>" \
							"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"back\" name=\"back\" value=\"<script>w('bck')</script>\" onclick=\"history.back()\";></div>\n"));
				break;

			case FAN_ERROR_NOT_COMPLETED :
				server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_WARNING_URL "\"></div>\n" \
							"<div class=\"message\"><script>w('ucj')</script></div>" \
							"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"back\" name=\"back\" value=\"<script>w('bck')</script>\" onclick=\"history.back()\";></div>\n"));
				break;

			case FAN_ERROR_CONFIG_FAILED :
				server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_ERROR_URL "\"></div>\n" \
							"<div class=\"message\"><script>w('ncf')</script></div>\n" \
							"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"back\" name=\"back\" value=\"<script>w('bck')</script>\" onclick=\"history.back()\";></div>\n"));
				break;

			default :
				server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_WARNING_URL "\"></div>\n" \
							"<div class=\"message\"><script>w('urs')</script></div>" \
							"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"back\" name=\"back\" value=\"<script>w('bck')</script>\" onclick=\"history.back();\"></div>\n"));
				break;
		}

		server.sendContent(	F("		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"finish\" name=\"finish\" value=\"<script>w('fin')</script>\"></div>\n" \
					"		</div>\n" \
					"	</form>\n"));
	} else {
		Serial.printf_P("Fan discover step 1: unplug\n");
		server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_POWER_OFF_URL "\"></div>\n" \
					"		<div class=\"message\"><script>w('fup')</script></div>\n" \
					"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"next\" name=\"plugin\" value=\"\"></div>\n" \
					"		</div>\n" \
					"	</form>\n"));
	}
	htmlFooter();
}

/*
  Configuration wizard step 6 - Manual fan configuration
*/
void htmlConfigWizardFanManual(void) {
	char		str_network_id[9] = {0},
			str_my_device_id[3] = {0},
			str_main_unit_id[3] = {0};

	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.hasArg("next")) {
		config.fan_my_device_type = FAN_TYPE_REMOTE_CONTROL;	// Generate random device_id; don't use 0x00 and 0xFF
		config.fan_my_device_id = strtoul(server.arg("myid").c_str(), 0, 16);
		config.fan_main_unit_type = FAN_TYPE_MAIN_UNIT;	// Generate random device_id; don't use 0x00 and 0xFF
		config.fan_main_unit_id = strtoul(server.arg("mainid").c_str(), 0, 16);
		config.fan_network_id = strtoul(server.arg("address").c_str(), 0, 16);

		nrf905->setModeIdle();		// Set the nRF905 to idle mode
		nrf905->setRxAddress(config.fan_network_id);
		nrf905->setTxAddress(config.fan_network_id);
		nrf905->writeTxAddress();
		nrf905->encodeConfigRegisters();
		// TODO: some sort of ping to the main unit to check if the network_id and main_unit_id is correct
		nrf905->writeConfigRegisters();
		Serial.printf_P("Fan configuration set: netid=%08X myid=%02X mainid=%02X\n", config.fan_network_id, config.fan_my_device_id, config.fan_main_unit_id);
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_REBOOT_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
	}

	uint32ToString(config.fan_network_id, str_network_id);
	uint8ToString(config.fan_my_device_id, str_my_device_id);
	uint8ToString(config.fan_main_unit_id, str_main_unit_id);
	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgfanm\" action=\"/cfgfanm\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n"));
	if (server.hasArg("next")) {
		server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_ERROR_URL "\"></div>\n" \
					"<div class=\"message\"><script>w('ncf')</script></div>"));
	} else {
		server.sendContent(	F("		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_FAN_URL "\"></div>\n"));
	}
	server.sendContent(	F("		<fieldset><legend><script>w('fpm')</script></legend>\n"));
	server.sendContent(	"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"address\"><script>w('fpm')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"address\" maxlength=\"8\" name=\"address\" value=\"" + String(str_network_id) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"myid\"><script>w('mid')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"myid\" maxlength=\"2\" name=\"myid\" value=\"" + String(str_my_device_id) + "\"></div><div class=\"form-icon\"><img alt=\"\" title=\"<script>w('rnd')</script>\" class=\"icon-white icon-password\" src=\"" ICON_DICE_URL "\" id=\"icon_my_device_id\" onclick=\"rndid('myid');\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"mainid\"><script>w('fid')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"mainid\" maxlength=\"2\" name=\"mainid\" value=\"" + String(str_main_unit_id) + "\"></div><div class=\"form-icon\"></div></div>\n");
	server.sendContent(	F(	"		</fieldset>\n" \
				"		<div class=\"form-row\"><input type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
   Configuration wizard step 7 - Advanced settings
*/
void htmlConfigWizardAdvanced(void) {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.hasArg("next")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_REBOOT_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	if (server.method() == HTTP_POST) {
		/* TODO input validation */
		if (server.hasArg("ntpserver"))
			strncpy(config.ntp_server, server.arg("ntpserver").c_str(), MAX_NTPSERVER);
		else
			strncpy(config.ntp_server, DEFAULT_NTP_SERVER, MAX_NTPSERVER);
		if (server.hasArg("ntpoffset"))
			config.ntp_offset = server.arg("ntpoffset").toInt();
		else
			config.ntp_offset = DEFAULT_NTP_OFFSET;
		if (server.hasArg("ntpinterval"))
			config.ntp_interval = server.arg("ntpinterval").toInt();
		else
			config.ntp_interval = DEFAULT_NTP_INTERVAL;
		if (server.hasArg("ota_password"))
			strncpy(config.ota_password, server.arg("ota_password").c_str(), MAX_OTA_PASSWORD);
		else
			strncpy(config.ota_password, DEFAULT_OTA_PASSWORD, MAX_OTA_PASSWORD);
		if (server.hasArg("ota_port"))
			config.ota_port = server.arg("ota_port").toInt();
		else
			config.ota_port = DEFAULT_OTA_PORT;
	}

	if (server.hasArg("next")) {
		server.sendHeader(FPSTR(HTTP_HEADER_LOCATION), FPSTR(HTML_CONFIG_REBOOT_URL));
		server.sendHeader(FPSTR(HTTP_HEADER_CACHE_CONTROL), FPSTR(HTTP_HEADER_NO_CACHE));
		server.send(HTTP_CODE_SEE_OTHER);
		return;
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgadvn\" action=\"/cfgadvn\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_SETTINGS_URL "\"></div>\n"));
#if NRF905API_NTP == 1
	server.sendContent(	"		<fieldset><legend><script>w('ntp')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpserver\"><script>w('nts')</script></label></div><div class=\"form-input\"><input type=\"text\" id=\"ntpserver\" maxlength=\"" + String(sizeof(config.ntp_server) - 1) + "\" name=\"ntpserver\" value=\"" + String(config.ntp_server) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpoffset\"><script>w('nto')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ntpoffset\" maxlength=\"" + String(sizeof(config.ntp_offset)) + "\" name=\"ntpoffset\" value=\"" + String(config.ntp_offset) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ntpinterval\"><script>w('nts')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ntpinterval\" maxlength=\"" + String(sizeof(config.ntp_interval)) + "\" name=\"ntpinterval\" value=\"" + String(config.ntp_interval) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n");
#endif
#if NRF905API_OTA == 1
	server.sendContent(	"		<fieldset><legend><script>w('ous')</script></legend>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ota_password\"><script>w('phs')</script></label></div><div class=\"form-input\"><input type=\"password\" id=\"ota_password\" maxlength=\"" + String(sizeof(config.ota_password) - 1) + "\" name=\"ota_password\" value=\"" + String(config.ota_password) + "></div><div class=\"form-icon\"><img alt=\"\" title=\"\" class=\"icon-white icon-password\" src=\"" ICON_VISIBILITY_OFF_URL "\" id=\"icon_ota_password\" onclick=\"changeInputType('ota_password', 'icon_ota_password');\"></div></div>\n" \
				"			<div class=\"form-row\"><div class=\"form-label\"><label for=\"ota_port\"><script>w('prt')</script></label></div><div class=\"form-input\"><input type=\"number\" id=\"ota_port\"  name=\"ota_port\" min=\"1\" max=\"65535\" value=\"" + String(config.ota_port) + "\"></div><div class=\"form-icon\"></div></div>\n" \
				"		</fieldset>\n");
#endif
	server.sendContent(	F("		<div class=\"form-row\"><input type=\"submit\" id=\"next\" name=\"next\" value=\"\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

/*
   Configuration wizard step 8 - Reboot
*/
void htmlConfigWizardReboot(void) {
	if ((strlen(config.http_username) != 0) && (strlen(config.http_password) != 0)) {
		if (!server.authenticate(config.http_username, config.http_password)) {
			return server.requestAuthentication(DIGEST_AUTH, HTTP_HTML_AUTH_REALM, HTTP_HTML_AUTH_FAILED);
		}
	}

	if (server.method() == HTTP_POST) {
		if (server.hasArg("reboot")) {
			writeNVRAM();
			board->CommitNVRAM();
			htmlFooter();
			delay(5000);
			board->reset();
			return;
		}
	}

	htmlHeader();
	server.sendContent(	F("	<form name=\"cfgboot\" action=\"/cfgboot\" method=\"post\">\n" \
				"		<div class=\"form-list\">\n" \
				"		<div class=\"form-row\"><img alt=\"\" class=\"icon-white icon-big\" src=\"" ICON_REBOOT_URL "\"></div>\n" \
				"		<div class=\"message\"><script>w('dnc')</script></div>\n" \
				"		<div class=\"form-row\"><input type=\"submit\" autofocus id=\"reboot\" name=\"reboot\" value=\"\" onclick=\"self.close();\"></div>\n" \
				"		</div>\n" \
				"	</form>\n"));
	htmlFooter();
}

