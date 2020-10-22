#include "fan.h"						// network_link_id
#include "nvram.h"
#include "nRF905API.h"						// config
#include "src/hardware/nrf905.h"				// NRF905_MAX_FRAMESIZE, nrf905->*
#include "src/language/language.h"				// DEFAULT_LANGUAGE
#include <EEPROM.h>						// EEPROM.read()

extern Board * board;						// board->*: Defined in nRF905API.ino
extern NVRAMBuffer			config;		// config: Defined in nRF905API.ino

bool readNVRAM(void) {
	board->ReadNVRAM((uint8_t *) &config, 0, sizeof(config));
//	dumpNVRAM();

	if (config.signature == NVRAM_SIGNATURE) {
		if (config.version == NVRAM_VERSION) {
			nrf905->setModeIdle();
			nrf905->restoreConfigRegisters(config.nrf905_registers);
			nrf905->decodeConfigRegisters();
			nrf905->writeConfigRegisters();
			nrf905->writeTxPayload(config.nrf905_tx_payload);
			nrf905->setTxAddress(config.nrf905_tx_address);
			nrf905->writeTxAddress();
			Serial.printf("nvram: read ok\n");		// TODO remove this
			return true;
		} else {
			Serial.printf("nvram: read data version 0x%04X is incompatible with 0x%04X\n", config.version, NVRAM_VERSION); // TODO update nvram data on firmware update
		}
	} else {
		Serial.printf("nvram: read invalid signature\n");	// TODO remove this
	}
	return false;
}

void writeNVRAM(void) {
	config.signature = NVRAM_SIGNATURE;
	config.version = NVRAM_VERSION;
	nrf905->setModeIdle();
	nrf905->readConfigRegisters();
	nrf905->decodeConfigRegisters();
	nrf905->backupConfigRegisters(config.nrf905_registers);
	nrf905->readTxPayload(config.nrf905_tx_payload);
	config.nrf905_tx_address = nrf905->getTxAddress();

	board->WriteNVRAM((uint8_t *) &config, 0, sizeof(config));
	board->CommitNVRAM();
//	dumpNVRAM();

	Serial.printf("nvram: write ok\n");	// TODO remove this
}

void dumpNVRAM(void) {
	size_t	i;

	Serial.printf("signature=%04X\n", config.signature);
	Serial.printf("version=%04X\n", config.version);
	Serial.printf("language=%c%c\n", config.language[0], config.language[1]);
	Serial.printf("hostname=%s\n", config.wifi_hostname);
	Serial.printf("ssid=%s\n", config.wifi_ssid);
	Serial.printf("password=%s\n", config.wifi_password);
	Serial.printf("ntp_server=%s\n", config.ntp_server);
	Serial.printf("ntp_offset=%i\n", config.ntp_offset);
	Serial.printf("ntp_interval=%i\n", config.ntp_interval);
	Serial.printf("http_username=%s\n", config.http_username);
	Serial.printf("http_password=%s\n", config.http_password);
	Serial.printf("ota_port=%i\n", config.ota_port);
	Serial.printf("ota_password=%s\n", config.ota_password);
	Serial.printf("nrf905_registers=");
	for (i = 0; i < sizeof(config.nrf905_registers); i++)
		Serial.printf("%02X:", config.nrf905_registers[i]);
	Serial.printf("\n");
	Serial.printf("nrf905_tx_payload=");
	for (i = 0; i < sizeof(config.nrf905_tx_payload); i++)
		Serial.printf("%02X:", config.nrf905_tx_payload[i]);
	Serial.printf("\n");
	Serial.printf("nrf905_tx_address=%08X\n", config.nrf905_tx_address);
	Serial.printf("fan_network_id=%08X\n", config.fan_network_id);
	Serial.printf("fan_my_device_type=%02X\n", config.fan_my_device_type);
	Serial.printf("fan_my_device_id=%02X\n", config.fan_my_device_id);
	Serial.printf("fan_main_unit_type=%02X\n", config.fan_main_unit_type);
	Serial.printf("fan_main_unit_id=%02X\n", config.fan_main_unit_id);
	Serial.printf("EEPROM=");
	for (i = 0; i < NVRAM_SIZE; i++)
		Serial.printf("%02X:", EEPROM.read(i));
	Serial.printf("\n");
}

void loadNVRAMDefaults(void) {
	config.signature = NVRAM_SIGNATURE;
	config.version = NVRAM_VERSION;
	strncpy_P(config.language, SELECTED_LANGUAGE, MAX_LANGUAGE);
	strncpy_P(config.wifi_hostname, DEFAULT_HOSTNAME, MAX_WIFI_HOSTNAME);
	memset(config.wifi_ssid, 0, sizeof(config.wifi_ssid));
	memset(config.wifi_password, 0, sizeof(config.wifi_password));
	strncpy_P(config.ntp_server, DEFAULT_NTP_SERVER, MAX_NTPSERVER);
	config.ntp_offset = DEFAULT_NTP_OFFSET;
	config.ntp_interval = DEFAULT_NTP_INTERVAL;
	strncpy_P(config.http_username, DEFAULT_HTTP_USERNAME, MAX_HTTP_USERNAME);
	strncpy_P(config.http_password, DEFAULT_HTTP_PASSWORD, MAX_HTTP_PASSWORD);
	config.ota_port = DEFAULT_OTA_PORT;
	strncpy_P(config.ota_password, DEFAULT_OTA_PASSWORD, MAX_OTA_PASSWORD);
	memset(&config.nrf905_tx_payload, 0, sizeof(config.nrf905_tx_payload));
	config.nrf905_tx_address = network_link_id;
	config.fan_network_id = 0x00000000;
	config.fan_my_device_type = 0x00;
	config.fan_my_device_id = 0x00;
	config.fan_main_unit_type = 0x00;
	config.fan_main_unit_id = 0x00;

	Serial.printf("nvram: load defaults ok\n");	// TODO remove this
}

void clearNVRAM(void) {
	memset(&config, 0xFF, sizeof(config));

//	board->ClearNVRAM();
	board->WriteNVRAM((uint8_t *) &config, 0, sizeof(config));
	board->CommitNVRAM();

	Serial.printf("nvram: clear ok\n");	// TODO remove this
}

bool checkNVRAMIsValidData(void) {
	return ((config.signature == NVRAM_SIGNATURE) && (config.version == NVRAM_VERSION));
}

