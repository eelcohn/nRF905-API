 /*
  nRF905 API - API to control a nRF905 transceiver module
  written by Eelco Huininga 2020
*/

#include "nRF905API.h"
#include "board.h"
#include "fan.h"						// Fan class
#include "nvram.h"						// NVRAMBuffer
#include "ssdp.h"
#include "utils.h"
#include "src/api/json_api.h"
#include "src/hardware/nrf905.h"
#include "src/html/html.h"
#include "src/html/configwizard.h"
#include "src/html/javascript.h"				// javascriptAddHandlers()
#include "src/html/stylesheet.h"				// stylesheetAddHandlers()
#include "src/html/icons.h"					// iconsAddHandlers()

#include <DNSServer.h>
#include <EEPROM.h>
#if NRF905API_NTP == 1
#include <NTPClient.h>
#include <WiFiUdp.h>
#endif
#if NRF905API_OTA == 1
#include <ArduinoOTA.h>
#endif

#if NRF905API_NTP == 1
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
#endif
DNSServer dnsServer;

Board *	board = new Board();
nRF905 *	nrf905 = new nRF905(PIN_AM, PIN_CD, PIN_CE, PIN_DR, PIN_PWR, PIN_TXEN, PIN_SPICS, NRF905_SPI_FREQUENCY);
Fan *		fan = new Fan();
NVRAMBuffer	config;
bool		rx_buffer_overflow;
size_t		rxnum;
uint8_t	txbuffer[NRF905_MAX_FRAMESIZE] = {0},
		rxbuffer_overflow[NRF905_MAX_FRAMESIZE] = {0},
		rxbuffer[rxbuffersize][NRF905_MAX_FRAMESIZE] = {0};
unsigned long	startTime;



/*
    Setup
*/

void setup() {
	uint8_t	wifi_connect_retries;
	
	delay(2000);			// Delay so the serial monitor can start up TODO remove in final release

	// Set up hardware board
	board->setOnBoardLED(false);	// Turn off on-board LED
	board->setADCtoVccMode();	// Set ADC to on-board Vcc measurement

	// Setup the serial connection
	Serial.begin(SERIAL_SPEED);
	Serial.printf("\n%s version %s\n", firmware_title, firmware_version);

	// Setup nRF905
	if (nrf905->init(NRF905_XTAL_FREQUENCY, NRF905_CLKOUT_FREQUENCY, false) == false)
		Serial.printf("nRF905: config failed\n");
	else
		Serial.printf("nRF905: config ok\n");
	nrf905->setModeIdle();

	// Setup EEPROM
	EEPROM.begin(NVRAM_SIZE);

	// Try to connect to WiFi network
	WiFi.mode(WIFI_OFF);
	WiFi.softAPdisconnect(true);

	if (readNVRAM() == true) {
		// Connect to WiFi network
		WiFi.mode(WIFI_STA);
		WiFi.begin(config.wifi_ssid, config.wifi_password);
		board->set_hostname(config.wifi_hostname);

		Serial.printf("WiFi: Connecting to %s", config.wifi_ssid);
		wifi_connect_retries = 0;
		while ((WiFi.status() != WL_CONNECTED) && (wifi_connect_retries < MAX_WIFI_CONNECT_RETRIES)) {
			board->setOnBoardLED(!board->getOnBoardLED());	// Toggle onboard LED
			delay(500);
			Serial.printf(".");
			wifi_connect_retries++;
		}
		if (wifi_connect_retries >= MAX_WIFI_CONNECT_RETRIES) {
			Serial.printf(" failed\n");
		} else {
			Serial.printf(" connected\n");
		}
	}

	if (WiFi.status() != WL_CONNECTED) {
		board->setOnBoardLED(true);	// Turn on on-board LED to signal WiFi AP-mode
		Serial.printf("Switching to AP mode\n");
		WiFi.mode(WIFI_OFF);
		WiFi.softAPdisconnect(true);
		delay(1000);
		WiFi.mode(WIFI_AP);
//		if(!WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0))) {
//			Serial.printf("SoftAP config failed\n");
//		}
		Serial.printf(WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD) ? "WiFi SoftAP: setup done\n" : "WiFi SoftAP: setup failed\n");
		delay(200);
		dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
		Serial.printf("WiFi SoftAP: IP address: ");
		Serial.println(WiFi.softAPIP());

		/* Set default values for NVRAM */
		if (checkNVRAMIsValidData() == false) {
			loadNVRAMDefaults();
		}
		strncpy(config.http_username, "", MAX_HTTP_USERNAME);
		strncpy(config.http_password, "", MAX_HTTP_PASSWORD);

		server.begin();
		htmlAddHandlers();
		htmlConfigWizardAddHandlers();
		javascriptAddHandlers();
		stylesheetAddHandlers();
		iconsAddHandlers();
		startTime = millis();

		// TODO why can I receive data while the isrRxHandler isn't initialized?!?!

		while ((millis() - startTime) < MAX_WIFI_AP_ONLINE_TIME) {	// Shut down WiFi AP after 10 minutes
			dnsServer.processNextRequest();
			server.handleClient();
			delay(10);
		}
		WiFi.mode(WIFI_OFF);
		nrf905->setModePowerDown();
		Serial.printf("WiFi SoftAP: switched off due to inactivity.");
//		esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
//		while(1)
//			esp_deep_sleep(5000000);
		while(1)
			board->deepSleep(0);
	}

	board->setOnBoardLED(false);	// Turn off on-board LED to signal WiFi client-mode
	Serial.println("WiFi: IP address: " + WiFi.localIP().toString());
#if NRF905API_IPV6 == 1
	Serial.printf("WiFi: IP6 address: ");
	if (board->enable_IPv6() == true)
		Serial.println(board->get_localIPv6());
	else
		Serial.printf("n/a\n");
#endif

#if NRF905API_MDNS == 1
	Serial.printf("mDNS: ");
	if (MDNS.begin(config.wifi_hostname)) {
#if NRF905API_HTTPS == 1
		MDNS.addService("https", "tcp", DEFAULT_HTTPS_PORT);
#else
		MDNS.addService("http", "tcp", DEFAULT_HTTP_PORT);
#endif
		Serial.printf("started\n");
	} else {
		Serial.printf("failed\n");
	}
#endif

#if NRF905API_NTP == 1
	Serial.printf("NTP client: ");
	timeClient.begin();
	timeClient.setPoolServerName(config.ntp_server);
	timeClient.setTimeOffset(config.ntp_offset);
	timeClient.setUpdateInterval(config.ntp_interval);
	timeClient.forceUpdate();
	Serial.printf("started\n");
#endif

#if NRF905API_OTA == 1
	Serial.printf("ota: ");
	ArduinoOTA.setPort(config.ota_port);
	ArduinoOTA.setHostname(config.wifi_hostname);
	ArduinoOTA.setPasswordHash(config.ota_password);
	ArduinoOTA.begin();
	Serial.printf("started\n");
#endif

#if NRF905API_HTTPS == 1
	// Set the certificates from PMEM (if using DRAM remove the _P from the call)
	server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
//	server.getServer().setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
#ifdef ARDUINO_ARCH_ESP8266
	server.getServer().setBufferSizes(2048, 837);
#endif
#endif

	// Start the HTTP server
	Serial.printf("http: ");
	server.onNotFound(json_sendNotFound);
	htmlAddHandlers();
	htmlConfigWizardAddHandlers();
	javascriptAddHandlers();
	stylesheetAddHandlers();
	apiv1_AddHandlers();
	apiv2_AddHandlers();
	apitest_AddHandlers();
	iconsAddHandlers();
	server.begin();
	Serial.printf("started\n");

#if NRF905API_SSDP == 1
	Serial.printf("SSDP: ");

	server.on(SSDP_DESCRIPTION_URL,			HTTP_GET, []() {SSDP.schema(server.client());	});

	SSDP.setSchemaURL(SSDP_SCHEMA_URL);
#if NRF905API_HTTPS == 1
	SSDP.setHTTPPort(DEFAULT_HTTPS_PORT);
#else
	SSDP.setHTTPPort(DEFAULT_HTTP_PORT);
#endif
	SSDP.setName(firmware_title);
	SSDP.setModelName(firmware_title);
	SSDP.setModelNumber(firmware_version);
	SSDP.setModelURL(HTML_CONFIG_URL);
	SSDP.setManufacturer(SSDP_MANUFACTURER);
	SSDP.setManufacturerURL(SSDP_MANUFACTURER_URL);
	SSDP.setSerialNumber(String(board->get_cpu_id(), HEX));
	SSDP.setURL(HTML_CONFIG_URL);
	SSDP.begin();
	Serial.printf("started\n");
#endif

	rxnum = 0;
	rx_buffer_overflow = false;
	Serial.printf("nRF905: switching to receive mode\n");
	Serial.printf("mode=%i\n", nrf905->getMode());
	nrf905->setModeReceive();
	Serial.printf("mode=%i\n", nrf905->getMode());
	Serial.printf("Setup done\n");
}



/*
    Main loop
*/

void loop() {
	/* Handle incoming HTTP(S) requests */
	server.handleClient();

	/* Handle mDNS */
#if NRF905API_MDNS == 1
	MDNS.update();  // Handle mDNS updates
#endif

	/* Handle NTP */
#if NRF905API_NTP == 1
	timeClient.update();
#endif

	/* Handle OTA updates */
#if NRF905API_OTA == 1
	ArduinoOTA.handle();  // Handle OTA requests
#endif

	/* Handle onboard LED Carrier Detect timer */
	nrf905->update();
}

//void ICACHE_RAM_ATTR rxISR(void) {			// TODO move this to nrf905 Class
void IRAM_ATTR rxISR(void) {
#if PIN_CD == -1
//	cdISR();
	board->setOnBoardLED(true);			// Turn on on-board LED to signal Carrier Detect
	nrf905->startCDLED = millis();			// Set timer
#endif

	if (rx_buffer_overflow == false) {
		Serial.printf("Data received\n");
		nrf905->readRxPayload(rxbuffer[rxnum++]);
		rx_buffer_overflow = (rxnum == rxbuffersize);
	} else {
		nrf905->readRxPayload(rxbuffer_overflow);
		Serial.printf("Data received - not stored due to buffer overflow!\n");
	}
}

