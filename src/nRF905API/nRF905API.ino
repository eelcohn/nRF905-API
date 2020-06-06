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
#if NRF905API_ZEHNDER == 1
#include "zehnder.h"
#endif


#if NRF905API_NTP == 1
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, ntp_offset, ntp_updateinterval);
#endif

const size_t rxbuffersize = 100;		// A maximum of 100 received frames can be cached

DynamicJsonDocument jsonDocument(JSON_OBJECT_SIZE(rxbuffersize + 10));

Board * board = new Board();
nRF905 *nrf905 = new nRF905(PIN_AM, PIN_CD, PIN_CE, PIN_DR, PIN_PWR, PIN_TXEN, PIN_SPICS, SPIFrequency);
int err;
size_t rxnum;
uint8_t txbuffer[NRF905_MAX_FRAMESIZE] = {0}, rxbuffer[rxbuffersize][NRF905_MAX_FRAMESIZE] = {0};
bool rx_buffer_overflow;


void setup() {
	// Setup the serial connection
	Serial.begin(SERIAL_SPEED);
	Serial.println();
	Serial.print(firmware_title);
	Serial.print(" version ");
	Serial.println(firmware_version);

	// Connect to WiFi network
	WiFi.mode(WIFI_STA);
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
	Serial.printf("Starting ArduinoOTA... ");
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
#if NRF905API_ZEHNDER == 1
	server.on("/api/v1/zehnder/config.json", HTTP_GET, zehnder_handleConfig);
	server.on("/api/v1/zehnder/link.json", HTTP_GET, zehnder_handleLink);
	server.on("/api/v1/zehnder/setpower.json", HTTP_GET, zehnder_handlePower);
	server.on("/api/v1/zehnder/settimer.json", HTTP_GET, zehnder_handleTimer);
#endif
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
			if (rx_buffer_overflow == false) {
				Serial.println("Data received");
				nrf905->readRxPayload(rxbuffer[rxnum++]);
				if (rxnum == rxbuffersize)
					rx_buffer_overflow = true;
			} else
				Serial.println("Data received - not stored due to buffer overflow!");
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

/* Handler for the API v1 status call */
void handleAPIv1Status() {
	uint8_t rxpayloadbin[NRF905_MAX_FRAMESIZE], txpayloadbin[NRF905_MAX_FRAMESIZE];
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
		binToHexstring(rxpayloadbin, rxpayloadstr, sizeof(rxpayloadbin));
		binToHexstring(txpayloadbin, txpayloadstr, sizeof(txpayloadbin));
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

/* Handler for the API v1 Zehnder config call */
void zehnder_handleConfig() {
	String json = "";
	uint32_t network;

	Serial.println("/api/v1/zehnder/config.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if network parameter was given */
	if (server.hasArg("network")) {
		network = strtoul(server.arg("network").c_str(), 0, 16);
		err = nrf905->setTxAddress(network);
		nrf905->writeTxAddress();
		err = nrf905->setRxAddress(network);
	} else {
		sendReplyError(HTTP_CODE_OK, "Parameter 'network' is missing");
		return;
	}		

	/* Check if the main_unit_id parameter is present */
	if (server.hasArg("main_unit_id")) {
		/* Check if the device_id parameter is valid */
		if (!validHex(server.arg("main_unit_id").c_str())) {
			sendReplyError(HTTP_CODE_OK, "Parameter 'main_unit_id' is not valid hex");
			return;
		}
		zehnder.main_unit_id = strtoul(server.arg("main_unit_id").c_str(), 0, 16);
	} else {
		sendReplyError(HTTP_CODE_OK, "Parameter 'main_unit_id' is missing");
		return;
	}

	/* Check if the device_id parameter is present */
	if (server.hasArg("device_id")) {
		/* Check if the device_id parameter is valid */
		if (!validHex(server.arg("device_id").c_str())) {
			sendReplyError(HTTP_CODE_OK, "Parameter 'device_id' is not valid hex");
			return;
		}
		zehnder.device_id = strtoul(server.arg("device_id").c_str(), 0, 16);
	} else {
		sendReplyError(HTTP_CODE_OK, "Parameter 'main_unit_id' is missing");
		return;
	}

	zehnder.main_unit_type = 0x01;
	zehnder.device_type = 0x03;

	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->setFrequency(868400000);
	nrf905->setCRC(true);
	nrf905->setCRCbits(16);
	nrf905->setRxReducedPower(false);
	nrf905->setTxPower(10);
	nrf905->setRxAddressWidth(4);
	nrf905->setTxAddressWidth(4);
	nrf905->setRxPayloadWidth(16);
	nrf905->setTxPayloadWidth(16);
	nrf905->encodeConfigRegisters();
	if (nrf905->writeConfigRegisters() == false) {
		nrf905->setModeReceive();
		sendReplyError(HTTP_CODE_OK, "Failed to write config to nRF905");
		return;
	}
	delay(10);			// Not sure if this is nessecary, but just as a precaution
	nrf905->setModeReceive();	// Enable Rx mode on the nRF905

	jsonDocument["result"] = "ok";
	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
}

/* Handler for the API v1 link call */
void zehnder_handleLink() {
	String json = "";
	unsigned long startRxTime, startTxTime;
	uint32_t network_id = 0x00000000, timeout;
	uint8_t device_id, frametype, device_type = 0x03, rx_type, tx_type = 0x00, rx_id, tx_id = 0x00;
	uint8_t payload[ZEHNDER_FRAMESIZE] = {0x04, 0x00, device_type, 0x00, 0xfa, ZEHNDER_NETWORK_JOIN_ACK, 0x04, 0xa5, 0x5a, 0x5a, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00};
	char rxhexstring[65] = {0};
	size_t i;
	bool success;

	Serial.println("/api/v1/zehnder/link.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if the device_id parameter is present */
	if (server.hasArg("device_id")) {
		/* Check if the device_id parameter is valid */
		if (!validHex(server.arg("device_id").c_str())) {
			sendReplyError(HTTP_CODE_OK, "Parameter 'device_id' is not valid hex");
			return;
		}
		device_id = strtoul(server.arg("device_id").c_str(), 0, 16);
	} else {
		device_id = random(1, 255);	// Generate random device_id; don't use 0x00 and 0xFF
	}
	payload[0x03] = device_id;	// TODO check if my device ID is not the same as the Zehnder unit's ID
	Serial.println((String)"Debug: My ID is 0x"+(String(device_id, HEX))+". Searching...");

	/* Check if a timeout parameter was given */
	if (server.hasArg("timeout"))
		timeout = server.arg("timeout").toInt();
	else
		timeout = 20000; // 30 seconds default

	/* Set transmit and receive network address to Link address */
	nrf905->setModeIdle();		// Set the nRF905 to idle mode
	nrf905->setFrequency(868400000);
	nrf905->setCRC(true);
	nrf905->setCRCbits(16);
	nrf905->setRxReducedPower(false);
	nrf905->setTxPower(10);
	nrf905->setRxAddressWidth(4);
	nrf905->setTxAddressWidth(4);
	nrf905->setRxPayloadWidth(16);
	nrf905->setTxPayloadWidth(16);

	nrf905->setRxAddress(network_link_id);
	nrf905->setTxAddress(network_link_id);
	nrf905->writeTxAddress();
	nrf905->encodeConfigRegisters();
	if (nrf905->writeConfigRegisters() == false) {
		nrf905->setModeReceive();
		sendReplyError(HTTP_CODE_OK, "Failed to write config to nRF905");
		return;
	}
	delay(10);			// Not sure if this is nessecary, but just as a precaution
	nrf905->setModeReceive();	// Enable Rx mode on the nRF905

	/* Transmit the payload */
	nrf905->writeTxPayload(payload);
	startTxTime = millis();
	success = false;
	rxnum = 0;	// TODO: display warning in case there's still unprocessed rxdata
	while ((success == false) && ((millis() - startTxTime) < timeout)) {
		nrf905->startTx(0, 1);

		/* Receive reply, if any is available */
		nrf905->setModeReceive();
		startRxTime = millis();
		while ((millis() - startRxTime) < 1000) {
			if (board->readPin(PIN_DR) == HIGH) {
				nrf905->readRxPayload(rxbuffer[rxnum]);
				rx_type = rxbuffer[rxnum][0x00];
				rx_id = rxbuffer[rxnum][0x01];
				tx_type = rxbuffer[rxnum][0x02];
				tx_id = rxbuffer[rxnum][0x03];
				frametype = rxbuffer[rxnum][0x05];	// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
				switch (frametype) {
					case 0x06 :	// Received linking request from main unit
						network_id = (rxbuffer[rxnum][0x07] | (rxbuffer[rxnum][0x08] << 8) | (rxbuffer[rxnum][0x09] << 16) | (rxbuffer[rxnum][0x0A] << 24));
						Serial.println((String)"Debug: Found unit type 0x"+(String(tx_type, HEX))+" with ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
						payload[0x00] = 0x01;			// Set type to main unit
						payload[0x01] = tx_id;			// Set ID to the ID of the main unit
						payload[0x05] = ZEHNDER_NETWORK_JOIN_REQUEST;			// Request to connect to network
						payload[0x07] = rxbuffer[rxnum][0x07];	// Request to connect to the received network ID
						payload[0x08] = rxbuffer[rxnum][0x08];
						payload[0x09] = rxbuffer[rxnum][0x09];
						payload[0x0A] = rxbuffer[rxnum][0x0A];
						nrf905->setRxAddress(network_id);
						nrf905->setTxAddress(network_id);
						nrf905->writeTxAddress();
						nrf905->encodeConfigRegisters();
						nrf905->writeConfigRegisters();
						nrf905->writeTxPayload(payload);
						nrf905->startTx(0, 1);
						zehnder.main_unit_type = tx_type;
						zehnder.main_unit_id = tx_id;
						break;
					case 0x0B :	// Main unit link was succesful (acknowledge)
						if ((rx_type = device_type) && (rx_id == device_id) && (tx_type == zehnder.main_unit_type) && (tx_id == zehnder.main_unit_id)) {
							if (success != true) {
								Serial.println("Debug: Link successful to unit with ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
								startRxTime = millis(); // Wait 1 second more to make sure we receive all frames
								success = true;
							}
						} else {
							Serial.println("Debug: Received unknown link succes from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
						}
						break;
					default :
						Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(network_id, HEX)));
						break;
				}
				rxnum++;
			}
		}
	}

	if (success == true) {
		/* Send request to join network */
		Serial.println((String)"Debug: sending join request acknowledge 0x0b");
		payload[0x05] = ZEHNDER_FRAME_0B;	// 0x0B acknowledgee link successful
		payload[0x06] = 0x00;	// No parameters
		payload[0x07] = 0x00;	// Clear rest of payload buffer
		payload[0x08] = 0x00;
		payload[0x09] = 0x00;
		payload[0x0A] = 0x00;
		nrf905->writeTxPayload(payload);
		nrf905->startTx(0, 1);

		/* Wait for acknowledge that join network was successful */
		startRxTime = millis();
		success = false;
		while ((millis() - startRxTime) < 1000){
			if (board->readPin(PIN_DR) == HIGH) {
				nrf905->readRxPayload(rxbuffer[rxnum]);
				if (rxbuffer[rxnum][0x05] == 0x0D) {
					if (success != true)
						Serial.println((String)"Debug: received network join success 0x0d");
					if ((rxbuffer[rxnum][0x00] == tx_type) && (rxbuffer[rxnum][0x01] == tx_id) && (rxbuffer[rxnum][0x02] == tx_type) && (rxbuffer[rxnum][0x03] == tx_id)) {
						if (success != true) {
							Serial.println((String)"Debug: 0x0d sanity check: ok");
							success = true;
						}
					}
				}
				rxnum++;
			}
		}

		/* */
		if (success == true) {
			zehnder.main_unit_type = tx_type;
			zehnder.main_unit_id = tx_id;
			jsonDocument["result"] = "ok";
			Serial.println((String)"Debug: join successful");
		} else {
			jsonDocument["result"] = "failed";
			jsonDocument["errormsg"] = "Unable to complete join transaction";
			Serial.println((String)"Debug: unable to complete join transaction");
		}
		jsonDocument["network_id"] = "0x"+String(network_id, HEX);
		jsonDocument["remote_device_type"] = "0x"+String(tx_type, HEX);
		jsonDocument["remote_device_id"] = "0x"+String(tx_id, HEX);
	} else {
		Serial.println((String)"Debug: no networks found");
		jsonDocument["result"] = "no_networks_found";
	}
	jsonDocument["my_device_type"] = "0x"+String(device_type, HEX);
	jsonDocument["my_device_id"] = "0x"+String(device_id, HEX);

	JsonArray rxdata = jsonDocument.createNestedArray("rxdata");
	for (i = 0; i < rxnum; i++) {
		binToHexstring(rxbuffer[i], rxhexstring, nrf905->getRxPayloadWidth());
		rxdata.add(rxhexstring);
		memset(rxbuffer[i], 0, sizeof(rxbuffer[i]));
	}

	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
	rxnum = 0;
}

/* Handler for the API v1 set power call */
void zehnder_handlePower() {
	String json = "";
	unsigned long startRxTime;
	uint8_t frametype, power, rx_type, tx_type, rx_id, tx_id;
	char rxhexstring[65] = {0};
	size_t i;
	bool success;

	Serial.println("/api/v1/zehnder/setpower.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if the device_id parameter is present */
	if (!server.hasArg("power")) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'power' is missing");
		return;
	} else {
		if (server.arg("power") == "low")
			power = 0x01;
		else if (server.arg("power") == "medium")
			power = 0x02;
		else if (server.arg("power") == "high")
			power = 0x03;
		else {
			sendReplyError(HTTP_CODE_OK, "Invalid value for parameter 'power'");
			return;
		}
	}

	uint8_t payload[ZEHNDER_FRAMESIZE] = {zehnder.main_unit_type, zehnder.main_unit_id, zehnder.device_type, zehnder.device_id, 0xfa, ZEHNDER_FRAME_SETPOWER, 0x01, power, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	/* Transmit the payload */
	nrf905->writeTxPayload(payload);
	nrf905->startTx(0, 1);

	/* Receive reply, if any is available */
	success = false;
	nrf905->setModeReceive();
	startRxTime = millis();
	while ((millis() - startRxTime) < 1000) {
		if (board->readPin(PIN_DR) == HIGH) {
			nrf905->readRxPayload(rxbuffer[rxnum]);
			rx_type = rxbuffer[rxnum][0x00];
			rx_id = rxbuffer[rxnum][0x01];
			tx_type = rxbuffer[rxnum][0x02];
			tx_id = rxbuffer[rxnum][0x03];
			frametype = rxbuffer[rxnum][0x05];	// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case 0x02 :	// Retransmit of command by main unit (mesh networking)
					Serial.println((String)"Debug: received 0x02");
					break;

				case 0x05 :	// Retransmit of reply to command 0x07 from RFZ (mesh networking)
					Serial.println((String)"Debug: received 0x05");
					break;

				case 0x07 :	// Reply / info request???
					if ((rx_type = zehnder.device_type) && (rx_id == zehnder.device_id) && (tx_type == zehnder.main_unit_type) && (tx_id == zehnder.main_unit_id)) {
						Serial.println((String)"Debug: sending reply to 0x07");
						payload[0x05] = ZEHNDER_FRAME_05;	// Reply to 0x07
						payload[0x06] = 0x03;			// 3 parameters
						payload[0x07] = 0x54;			// ???
						payload[0x08] = 0x03;			// ???
						payload[0x09] = 0x20;			// ???
						nrf905->writeTxPayload(payload);
						nrf905->startTx(0, 1);
						success = true;
					}
					break;

				default :
					Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(nrf905->getRxAddress(), HEX)));
					break;
			}
			rxnum++;
		}
	}

	if (success == true) {
		jsonDocument["result"] = "ok";
		Serial.println((String)"Debug: command sent successful");
	} else {
		jsonDocument["result"] = "no_reply";
		jsonDocument["errormsg"] = "Command sent but no acknowledge was received";
		Serial.println((String)"Debug: command sent but no acknowledge was received");
	}

	JsonArray rxdata = jsonDocument.createNestedArray("rxdata");
	for (i = 0; i < rxnum; i++) {
		binToHexstring(rxbuffer[i], rxhexstring, nrf905->getRxPayloadWidth());
		rxdata.add(rxhexstring);
		memset(rxbuffer[i], 0, sizeof(rxbuffer[i]));
	}

	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
	rxnum = 0;
}

/* Handler for the API v1 set timer call */
void zehnder_handleTimer() {
	String json = "";
	unsigned long startRxTime;
	uint8_t frametype, power, minutes, rx_type, tx_type, rx_id, tx_id;
	char rxhexstring[65] = {0};
	size_t i;
	bool success;

	Serial.println("/api/v1/zehnder/settimer.json called");

	if (!server.authenticate(www_username, www_password)) {
		return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	}

	/* Check if the device_id parameter is present */
	if (!server.hasArg("power")) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'power' is missing");
		return;
	} else {
		if (server.arg("power") == "low")
			power = 0x01;
		else if (server.arg("power") == "medium")
			power = 0x02;
		else if (server.arg("power") == "high")
			power = 0x03;
		else {
			sendReplyError(HTTP_CODE_OK, "Invalid value for parameter 'power'");
			return;
		}
	}

	if (!server.hasArg("minutes")) {
		sendReplyError(HTTP_CODE_OK, "Parameter 'minutes' is missing");
		return;
	} else {
		minutes = server.arg("minutes").toInt();
	}

	uint8_t payload[ZEHNDER_FRAMESIZE] = {zehnder.main_unit_type, zehnder.main_unit_id, zehnder.device_type, zehnder.device_id, 0xfa, ZEHNDER_FRAME_SETTIMER, 0x02, power, minutes, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	/* Transmit the payload */
	nrf905->writeTxPayload(payload);
	nrf905->startTx(0, 1);

	/* Receive reply, if any is available */
	success = false;
	nrf905->setModeReceive();
	startRxTime = millis();
	while ((millis() - startRxTime) < 1000) {
		if (board->readPin(PIN_DR) == HIGH) {
			nrf905->readRxPayload(rxbuffer[rxnum]);
			rx_type = rxbuffer[rxnum][0x00];
			rx_id = rxbuffer[rxnum][0x01];
			tx_type = rxbuffer[rxnum][0x02];
			tx_id = rxbuffer[rxnum][0x03];
			frametype = rxbuffer[rxnum][0x05];	// TODO: use a struct for rxbuffer, and do something like rxbuffer.frametype ?
			switch (frametype) {
				case 0x02 :	// Retransmit of command by main unit (mesh networking)
					Serial.println((String)"Debug: received 0x02");
					break;

				case 0x05 :	// Retransmit of reply to command 0x07 from RFZ (mesh networking)
					Serial.println((String)"Debug: received 0x05");
					break;

				case 0x07 :	// Reply / info request???
					if ((rx_type = zehnder.device_type) && (rx_id == zehnder.device_id) && (tx_type == zehnder.main_unit_type) && (tx_id == zehnder.main_unit_id)) {
						Serial.println((String)"Debug: sending reply to 0x07");
						payload[0x05] = ZEHNDER_FRAME_05;	// Reply to 0x07
						payload[0x06] = 0x03;			// 3 parameters
						payload[0x07] = 0x54;			// ???
						payload[0x08] = 0x03;			// ???
						payload[0x09] = 0x20;			// ???
						nrf905->writeTxPayload(payload);
						nrf905->startTx(0, 1);
						success = true;
					}
					break;

				default :
					Serial.println((String)"Debug: Received unknown frame type 0x"+(String(frametype, HEX))+" from ID 0x"+(String(tx_id, HEX))+" on network 0x"+(String(nrf905->getRxAddress(), HEX)));
					break;
			}
			rxnum++;
		}
	}

	if (success == true) {
		jsonDocument["result"] = "ok";
		Serial.println((String)"Debug: command sent successful");
	} else {
		jsonDocument["result"] = "no_reply";
		jsonDocument["errormsg"] = "Command sent but no reply was received";
		Serial.println((String)"Debug: command sent but no reply was received");
	}

	JsonArray rxdata = jsonDocument.createNestedArray("rxdata");
	for (i = 0; i < rxnum; i++) {
		binToHexstring(rxbuffer[i], rxhexstring, nrf905->getRxPayloadWidth());
		rxdata.add(rxhexstring);
		memset(rxbuffer[i], 0, sizeof(rxbuffer[i]));
	}

	serializeJson(jsonDocument, json);
	server.send(HTTP_CODE_OK, http_json_response, json);
	jsonDocument.clear();
	rxnum = 0;
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
