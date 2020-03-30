/*
  nRF905 Arduino API - API to control a nRF905 transceiver module
  written by Eelco Huininga 2020
*/

#include "nRF905API.h"
#include "utils.h"
#include "nRF905.h"
#include "board.h"

#include <ArduinoJson.h>
#if NRF905API_NTP == 1
#include <NTPClient.h>
#include <Time.h>
#include <WiFiUdp.h>
#endif
#if NRF905API_OTA == 1
#include <ArduinoOTA.h>
#endif


#if NRF905API_NTP == 1
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, ntp_offset, ntp_updateinterval);
#endif

const size_t rxbuffersize = 100;		// A maximum of 100 received frames can be cached

DynamicJsonDocument jsonDocument(JSON_OBJECT_SIZE(110));

Board * board = new Board();
nRF905 *nrf905 = new nRF905(PIN_AM, PIN_CD, PIN_CE, PIN_DR, PIN_PWR, PIN_TXEN, PIN_SPICS, SPIFrequency);
int err;
size_t rxnum;
uint8_t txbuffer[32] = {0}, rxbuffer[rxbuffersize][32] = {0};
bool rx_buffer_overflow;


void setup() {
	// Setup the serial connection
	Serial.begin(SERIAL_SPEED);
	Serial.println();
	Serial.print(firmware_title);
	Serial.print(" version ");
	Serial.println(firmware_version);

	// Connect to WiFi network
	WiFi.begin(_ssid, _password);
	board->set_hostname(_hostname);

	Serial.printf("\nStarting WiFi: Connecting to ");
	Serial.print(_ssid);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("WiFi: IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("WiFi: IP6 address: ");
	if (board->enable_IPv6() == true)
		Serial.println(board->get_localIPv6());
	else
		Serial.println("failed");
	Serial.print("WiFi: Signal strength (dBm): ");
	Serial.println(WiFi.RSSI());

#if NRF905API_MDNS == 1
	Serial.printf("Starting mDNS server... ");
	if (MDNS.begin(_hostname)) {
		Serial.printf("started\n");
#if NRF905API_HTTPS == 1
		MDNS.addService("https", "tcp", NRF905API_WWW_PORT);
#else
		MDNS.addService("http", "tcp", NRF905API_WWW_PORT);
#endif
	} else {
		Serial.printf("failed\n");
	}
#endif

#if NRF905API_NTP == 1
	Serial.printf("Starting NTP client...\n");
	timeClient.begin();
#endif

#if NRF905API_OTA == 1
	Serial.printf("Starting ArduinoOTAr... ");
	ArduinoOTA.setPort(NRF905API_OTA_PORT);
	ArduinoOTA.setHostname(_hostname);
	ArduinoOTA.setPasswordHash(NRF905API_OTA_PASSWORD);
	ArduinoOTA.begin();
	Serial.printf("started\n");
#endif

#if NRF905API_HTTPS == 1
	// Set the certificates from PMEM (if using DRAM remove the _P from the call)
	server.getServer().setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
#endif

	// Start the HTTP server
	Serial.printf("Starting HTTP server...\n");
	server.onNotFound(handleNotFound);
	server.on("/api/v1/config.json", HTTP_GET, handleAPIv1Config);
	server.on("/api/v1/receive.json", HTTP_GET, handleAPIv1Receive);
	server.on("/api/v1/send.json", HTTP_GET, handleAPIv1Send);
	server.on("/api/v1/status.json", HTTP_GET, handleAPIv1Status);
	server.on("/api/v1/systemconfig.json", HTTP_GET, handleAPIv1SystemConfig);
	server.begin();

#if NRF905API_SSDP == 1
	Serial.printf("Starting SSDP...\n");

	server.on("/description.xml", HTTP_GET, []() {
		SSDP.schema(server.client());
	});

	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(NRF905API_WWW_PORT);
	SSDP.setName(firmware_title);
	SSDP.setSerialNumber(String(board->get_cpu_id(), HEX));
	SSDP.setURL("index.html");
	SSDP.setModelName(firmware_title);
	SSDP.setModelNumber(firmware_version);
	SSDP.setModelURL("https://github.com/eelcohn/nRF905-API/");
	SSDP.setManufacturer("-");
	SSDP.setManufacturerURL("https://github.com/eelcohn/nRF905-API/");
	SSDP.begin();
#endif

	// Setup nRF905
	Serial.print("Starting nRF905...");
	reportResult(nrf905->init(16000000, 500000, false));

	Serial.println("nRF905: switching to receive mode");
	nrf905->setModeReceive();

	board->onBoardLED(false);	// Turn off on-board LED
	board->setADCtoVccMode();	// Set ADC to on-board Vcc measurement
	rxnum = 0;
	rx_buffer_overflow = false;
	Serial.println("Setup done");
}

void loop() {
	/* TODO remove this: temp stuff to check AM CD and DR inputs */
//	bool _am = board->readPin(PIN_AM);
//	bool _cd = board->readPin(PIN_CD);
	bool _dr = board->readPin(PIN_DR);
//	if ((_am == HIGH) || (_cd == HIGH) || (_dr == HIGH)) {
//		board->onBoardLED(true);	// Turn on on-board LED
		if (_dr == HIGH) {
			Serial.println("Data Ready");
			if (rx_buffer_overflow == false) {
				nrf905->readRxPayload(rxbuffer[rxnum++]);
				if (rxnum == rxbuffersize)
					rx_buffer_overflow = true;
			}
		}
//		if (_cd == HIGH)
//			Serial.print("Carrier Detect  ");
//		if (_dr == HIGH)
//			Serial.print("Data Ready");
//		Serial.println("");
//		board->onBoardLED(false);	// Turn off on-board LED
//	}

	/* Handle incoming HTTP(S) requests */
	server.handleClient();

	/* Handle mDNS */
#ifdef ARDUINO_ARCH_ESP8266
#if NRF905API_MDNS == 1
	MDNS.update();  // Handle mDNS updates
#endif
#endif

	/* Handle NTP */
#if NRF905API_NTP == 1
	timeClient.update();
#endif

	/* Handle OTA updates */
#if NRF905API_OTA == 1
	ArduinoOTA.handle();  // Handle OTA requests
#endif
}

/* Handler for the API v1 config call */
void handleAPIv1Config() {
	String json = "";
	uint8_t txaddrwidth, rxaddrwidth, txpayloadwidth, rxpayloadwidth;
	int8_t txpower;
	uint32_t frequency, rxaddr, txaddr;

	Serial.println("/api/v1/config.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if a frequency parameter was given */
	if (server.hasArg("frequency")) {
		frequency = server.arg("frequency").toInt();
		if (nrf905->setFrequency(frequency) == false) {
			sendReplyError(HTTP_CODE_OK, "Failed to set frequency");
			return;
		}
	}

	/* Check if crc parameter was given */
	if (server.hasArg("crc")) {
		if (server.arg("crc") == "off") {
			nrf905->setCRC(false);
		} else if (server.arg("crc") == "8") {
			nrf905->setCRC(true);
			err = nrf905->setCRCbits(8);
		} else if (server.arg("crc") == "16") {
			nrf905->setCRC(true);
			err = nrf905->setCRCbits(16);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'crc' is not valid");
			return;
		}		
	}

	/* Check if txpower parameter was given */
	if (server.hasArg("txpower")) {
		txpower = server.arg("txpower").toInt(); // TODO: do some checks to see if txpower is within int8_t type (-128...127)
		if (nrf905->setTxPower(txpower) == false) {
			sendReplyError(HTTP_CODE_OK, "Parameter 'txpower' is not valid");
			return;
		}		
	}

	/* Check if rxpower parameter was given */
	if (server.hasArg("rxpower")) {
		if (server.arg("rxpower") == "normal") {
			nrf905->setRxReducedPower(false);
		} else if (server.arg("rxpower") == "low") {
			nrf905->setRxReducedPower(true);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'rxpower' is not valid");
			return;
		}		
	}

	/* Check if txaddrwidth parameter was given */
	if (server.hasArg("txaddrwidth")) {
		txaddrwidth = server.arg("txaddrwidth").toInt();
		if (txaddrwidth == 1) {
			err = nrf905->setTxAddressWidth(1);
		} else if (txaddrwidth == 4) {
			err = nrf905->setTxAddressWidth(4);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'txaddrwidth' is not valid");
			return;
		}		
	}

	/* Check if rxaddrwidth parameter was given */
	if (server.hasArg("rxaddrwidth")) {
		rxaddrwidth = server.arg("rxaddrwidth").toInt();
		if (rxaddrwidth == 1) {
			err = nrf905->setRxAddressWidth(1);
		} else if (rxaddrwidth == 4) {
			err = nrf905->setRxAddressWidth(4);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'rxaddrwidth' is not valid");
			return;
		}		
	}

	/* Check if txpayloadwidth parameter was given */
	if (server.hasArg("txpayloadwidth")) {
		txpayloadwidth = server.arg("txpayloadwidth").toInt();
		if ((txpayloadwidth > 0) || (txpayloadwidth < 33)) {
			err = nrf905->setTxPayloadWidth(txpayloadwidth);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'txpayloadwidth' is not valid");
			return;
		}		
	}

	/* Check if rxpayloadwidth parameter was given */
	if (server.hasArg("rxpayloadwidth")) {
		rxpayloadwidth = server.arg("rxpayloadwidth").toInt();
		if ((rxpayloadwidth > 0) || (rxpayloadwidth < 33)) {
			err = nrf905->setRxPayloadWidth(rxpayloadwidth);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'rxpayloadwidth' is not valid");
			return;
		}		
	}

	/* Check if txaddr parameter was given */
	if (server.hasArg("txaddr")) {
		txaddr = strtoul(server.arg("txaddr").c_str(), 0, 16);

		if (nrf905->getTxAddressWidth() == 4) {
			err = nrf905->setTxAddress(txaddr);
			nrf905->writeTxAddress();
		} else if ((nrf905->getTxAddressWidth() == 1) && (txaddr < 256)) {
			err = nrf905->setTxAddress(txaddr);
			nrf905->writeTxAddress();
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'txaddr' is not valid");
			return;
		}		
	}

	/* Check if rxaddr parameter was given */
	if (server.hasArg("rxaddr")) {
		rxaddr = strtoul(server.arg("rxaddr").c_str(), 0, 16);

		if (nrf905->getRxAddressWidth() == 4) {
			err = nrf905->setRxAddress(rxaddr);
		} else if ((nrf905->getRxAddressWidth() == 1) && (rxaddr < 256)) {
			err = nrf905->setRxAddress(rxaddr);
		} else {
			sendReplyError(HTTP_CODE_OK, "Parameter 'rxaddr' is not valid");
			return;
		}		
	}

	/* Write config to the nRF905 chip */
	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->encodeConfigRegisters();
	if (nrf905->writeConfigRegisters() == false) {
		nrf905->setModeReceive();
		sendReplyError(HTTP_CODE_OK, "Failed to write config to nRF905");
		return;
	}
	delay(10);			// Not sure if this is nessecary, but just as a precaution
	nrf905->setModeReceive();	// Enable Rx mode on the nRF905

	/* Write to NVRAM, if requested */
	if (server.arg("nvram") == "true")
		nrf905->writeNVRAM();

	/* Write HTTP result */
	jsonDocument["result"] = "ok";
	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
}

void handleAPIv1SystemConfig() {
	String json = "";

	Serial.println("/api/v1/systemconfig.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if the MCU needs to be reset */
	if (server.arg("reset") == "true") {
		/* Write HTTP result */
		jsonDocument["result"] = "ok";
		serializeJson(jsonDocument, json);
		server.send(HTTP_CODE_OK, http_json_response, json);
		jsonDocument.clear();

		delay(250);
		nrf905->setModeIdle();
		board->reset();
	}

	jsonDocument["result"] = "ok";
	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
}

/* Handler for the API v1 send call */
void handleAPIv1Send() {
	String json = "";
	unsigned long startTime;
	uint32_t txaddr, retransmit, timeout;
	int payload_size;
	char rxhexstring[65] = {0};

	Serial.println("/api/v1/send.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if the addr parameter is present */
	if (!server.hasArg("txaddr")) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'txaddr' missing");
		return;
	}

	/* Check if the payload parameter is present */
	if (!server.hasArg("payload")) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'payload' missing");
		return;
	}

	/* Check if the addr parameter is valid */
	if (!validHex(server.arg("txaddr").c_str())) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'txaddr' is not valid hex");
		return;
	}
	txaddr = strtoul(server.arg("txaddr").c_str(), 0, 16);

	/* Check if the payload parameter is valid */
	if (!validHex(server.arg("payload").c_str())) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'payload' is not valid hex");
		return;
	}
	payload_size = hexstringToBin(server.arg("payload").c_str(), txbuffer, sizeof(txbuffer));
	if (payload_size < 0) {
		sendReplyError(HTTP_CODE_OK, "Can't convert 'payload' to hex");
		return;
	}

	/* Check if a duratiom parameter was given */
	if (server.hasArg("retransmit"))
		retransmit = server.arg("retransmit").toInt() * 1000;
	else
		retransmit = 0;	// No retransmission by default

	/* Check if a timeout parameter was given */
	if (server.hasArg("timeout"))
		timeout = server.arg("timeout").toInt();
	else
		timeout = 2000; // 2 seconds default

	if (txaddr != nrf905->getTxAddress()) {
		nrf905->setTxAddress(txaddr);
		nrf905->writeTxAddress();
	}

	/* Transmit the payload */
	nrf905->writeTxPayload(txbuffer);
	if (nrf905->startTx(retransmit, timeout) == false) {
		nrf905->setModeReceive();
		sendReplyError(HTTP_CODE_OK, "Frequency blocked");
		return;
	} else {
		jsonDocument["result"] = "ok";
		JsonArray rxdata = jsonDocument.createNestedArray("rxdata");

		/* Receive reply, if any is available */
		nrf905->setModeReceive();
		startTime = millis();
		while ((millis() - startTime) < timeout) {
			if (board->readPin(PIN_DR) == HIGH) {
//				JsonObject rxdata_0 = rxdata.createNestedObject();
				nrf905->readRxPayload(rxbuffer[0]);
				binToHexstring(rxbuffer[0], rxhexstring, nrf905->getRxPayloadWidth());
//				rxdata_0["timestamp"] = "na";
//				rxdata_0["data"] = rxhexstring;
				rxdata.add(rxhexstring);
				memset(rxbuffer[0], 0, sizeof(rxbuffer[0]));
			} else
				delay(5); // Wait 5 ms if nothing was received TODO find out what delay value should be used
		}
	}

	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);

	jsonDocument.clear();
	rxnum = 0;
}

/* Handler for the API v1 receive call */
void handleAPIv1Receive() {
	String json = "";
	char rxhexstring[65] = {0};
	size_t i;

	Serial.println("/api/v1/receive.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	if (rx_buffer_overflow == false)
		jsonDocument["result"] = "ok";
	else
		jsonDocument["result"] = "rx_buffer_overflow";

	JsonArray rxdata = jsonDocument.createNestedArray("rxdata");

	for (i = 0; i < rxnum; i++) {
//		JsonObject rxdata_0 = rxdata.createNestedObject();

		binToHexstring(rxbuffer[i], rxhexstring, nrf905->getRxPayloadWidth());
//		rxdata_0["timestamp"] = "na";
//		rxdata_0["data"] = rxhexstring;
		rxdata.add(rxhexstring);
	}

	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
	for (i = 0; i < rxnum; i++)
		memset(rxbuffer[i], 0, sizeof(rxbuffer[0]));
	rxnum = 0;
}

/* Handler for the API v1 status call */
void handleAPIv1Status() {
	uint8_t rxpayloadbin[32], txpayloadbin[32];
	char rxpayloadstr[65] = {0}, txpayloadstr[65] = {0};
	String json = "";

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	Serial.println("/api/v1/status.json called");

	jsonDocument["result"] = "ok";
	JsonObject system = jsonDocument.createNestedObject("system_status");
	system["board"] = board->get_arch();
	system["firmware_version"] = firmware_version;
	system["system_time"] = board->getDateTime();
	system["restart_reason"] = board->restartReason();
	system["cpu_id"] = String(board->get_cpu_id(), HEX);
	system["cpu_frequency"] = board->getCPUFreqMhz();
	system["vcc"] = (board->get_vcc() / 1000.0);
	system["sdk_version"] = board->getSdkVersion();
	system["core_version"] = board->get_core_version();
	system["core_revision"] = String(board->get_core_revision(), HEX);
	system["flash_chip_id"] = String(board->get_flash_chip_id(), HEX);
	system["flash_chip_speed"] = board->getFlashChipSpeed();
	system["flash_mode"] = board->getFlashMode();
	system["flash_chip_real_size"] = board->get_flash_chip_real_size();
	system["flash_chip_sdk_size"] = board->getFlashChipSdkSize();
	system["flash_crc"] = board->checkFlashCRC();
	system["sketch_size"] = board->getSketchSize();
	system["sketch_free_space"] = board->getFreeSketchSpace();
	system["sketch_md5"] = board->getSketchMD5();
	system["heap_free"] = board->getFreeHeap();
	system["heap_fragmentation"] = board->getHeapFragmentation();
	system["heap_maxfreeblocksize"] = board->getHeapMaxFreeBlockSize();
//		system_status["wifi_firmware_version"] = WiFi.firmwareVersion();

	JsonObject network = jsonDocument.createNestedObject("network");
	network["mac"] = String(WiFi.macAddress());
	network["hostname"] = board->get_hostname();
	network["ip4_addr"] = WiFi.localIP().toString();
	network["ip4_subnet"] = WiFi.subnetMask().toString();
	network["ip4_gateway"] = WiFi.gatewayIP().toString();
	network["ip4_dns"] = WiFi.dnsIP().toString();
	network["ip6_addr"] = board->get_localIPv6();
	network["rssi"] = WiFi.RSSI();
	network["ssid"] = WiFi.SSID();
	network["bssid"] =  WiFi.BSSIDstr();
	network["channel"] = WiFi.channel();

	JsonObject nrf905_status = jsonDocument.createNestedObject("nrf905");
	nrf905->readConfigRegisters();
	nrf905->decodeConfigRegisters();
	err = 0;
	if (err != 0) {
		Serial.println("Error reading nrf905 config");
		nrf905_status["status"] = "error";
	} else {
		nrf905_status["status"] = "ok";
		nrf905->readRxPayload(rxpayloadbin);
		nrf905->readTxPayload(txpayloadbin);
		binToHexstring(rxpayloadbin, rxpayloadstr, 32);
		binToHexstring(txpayloadbin, txpayloadstr, 32);
		if (nrf905->getStatus() < 16)
			nrf905_status["status_register"] = "0" + String(nrf905->getStatus(), HEX);
		else
			nrf905_status["status_register"] = String(nrf905->getStatus(), HEX);
		nrf905_status["clock_frequency"] = nrf905->getXtalFrequency();
		nrf905_status["clk_out_enabled"] = nrf905->getClkOut();
		nrf905_status["clk_out_frequency"] = nrf905->getClkOutFrequency();
		nrf905_status["rf_frequency"] = nrf905->getFrequency();
		nrf905_status["tx_power"] = nrf905->getTxPower();
		nrf905_status["rx_reduced_power"] = nrf905->getRxReducedPower();
		nrf905_status["rx_address_width"] = nrf905->getRxAddressWidth();
		nrf905_status["tx_address_width"] = nrf905->getTxAddressWidth();
		nrf905_status["rx_payload_width"] = nrf905->getRxPayloadWidth();
		nrf905_status["tx_payload_width"] = nrf905->getTxPayloadWidth();
		nrf905_status["rx_address"] = String(nrf905->getRxAddress(), HEX);
		nrf905_status["tx_address"] = String(nrf905->getTxAddress(), HEX);
		nrf905_status["rx_payload"] = rxpayloadstr;
		nrf905_status["tx_payload"] = txpayloadstr;
		nrf905_status["rx_buffered_frames"] = rxnum;
		nrf905_status["crc"] = nrf905->getCRC();
		nrf905_status["crc_bits"] = nrf905->getCRCbits();
	}
	nrf905->setModeReceive();
	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
}

/* Handler for 404: Not found */
void handleNotFound() {
	String json = "";

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	Serial.println("not_found called");
	sendReplyError(HTTP_CODE_NOT_FOUND, "");
}

/* Print the result of a nrf905 call to the serial interface */
void reportResult(bool result) {
	if (result == false)
		Serial.println("failed!");
	else
		Serial.println("ok");
}

/* Send an error API reply */
void sendReplyError(int httpCode, const char *errormsg) {
	String json = "";

	if (httpCode == HTTP_CODE_METHOD_NOT_ALLOWED)
		jsonDocument["result"] = "method_not_allowed";
	else if (httpCode == HTTP_CODE_NOT_FOUND)
		jsonDocument["result"] = "not_found";
	else
		jsonDocument["result"] = "failed";
	jsonDocument["errormsg"] = errormsg;
	serializeJson(jsonDocument, json);
	server.send(httpCode, http_json_response, json);
	jsonDocument.clear();
}
