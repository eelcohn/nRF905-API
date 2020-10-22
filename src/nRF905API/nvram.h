#ifndef __NVRAM_H__
#define __NVRAM_H__

/* NVRAM defaults */
#define NVRAM_SIZE			512					// Size of EEPROM
#define NVRAM_SIGNATURE		0xF905					// NVRAM signature
#define NVRAM_VERSION			0x0001					// NVRAM signature

/* Max size of config variables */
#define MAX_LANGUAGE			3
#define MAX_WIFI_HOSTNAME		33
#define MAX_WIFI_SSID			33
#define MAX_WIFI_PASSWORD		65
#define MAX_HTTP_USERNAME		33
#define MAX_HTTP_PASSWORD		33
#define MAX_NTPSERVER			33
#define MAX_OTA_PASSWORD		33

/* Default values for config variables */
#define DEFAULT_HOSTNAME		"nrf905-api"
#define DEFAULT_NTP_SERVER		"pool.ntp.org"				// NTP server - default server
#define DEFAULT_NTP_OFFSET		3600					// NTP server - default offset
#define DEFAULT_NTP_INTERVAL		86400					// NTP server - default update interval
#define DEFAULT_OTA_PORT		8233					// OTA TCP port
#define DEFAULT_OTA_PASSWORD		"c19555f1193996117d477eebde5c913e"	// MD5 password hash for password `nrf905`
#define DEFAULT_HTTP_USERNAME		"admin"  				// HTTP authentication: username
#define DEFAULT_HTTP_PASSWORD		"nrf905"				// HTTP authentication: password TODO use hash value (if possible)
#define DEFAULT_HTTP_PORT		80
#define DEFAULT_HTTPS_PORT		443

typedef struct {
	uint16_t			signature;				// NVRAM signature; always 0xF905
	uint16_t			version;				// NVRAM struct version; 0x0001 for this version
	char				language[MAX_LANGUAGE];		// Language of the user interface
	char				wifi_hostname[MAX_WIFI_HOSTNAME];	// WiFi Hostname
	char				wifi_ssid[MAX_WIFI_SSID];		// WiFi SSID
	char				wifi_password[MAX_WIFI_PASSWORD];	// WiFi Password
	char				ntp_server[MAX_NTPSERVER];		// NTP server address
	int				ntp_offset;				// NTP offset
	uint32_t			ntp_interval;				// NTP update interval
	char				http_username[MAX_HTTP_USERNAME];	// HTTP authentication - username
	char				http_password[MAX_HTTP_PASSWORD];	// HTTP authentication - password
	uint16_t			ota_port;				// OTA (Over The Air) firmware updates - port
	char				ota_password[MAX_OTA_PASSWORD];	// OTA (Over The Air) firmware updates - password hash
	uint8_t			nrf905_registers[10];			// nRF905 config registers
	uint8_t			nrf905_tx_payload[32];			// nRF905 Tx payload
	uint32_t			nrf905_tx_address;			// nRF905 Tx address
	uint32_t			fan_network_id;			// Fan (Zehnder/BUVA) network ID
	uint8_t			fan_my_device_type;			// Fan (Zehnder/BUVA) device type
	uint8_t			fan_my_device_id;			// Fan (Zehnder/BUVA) device ID
	uint8_t			fan_main_unit_type;			// Fan (Zehnder/BUVA) main unit type
	uint8_t			fan_main_unit_id;			// Fan (Zehnder/BUVA) main unit ID
} NVRAMBuffer;

bool readNVRAM(void);
void writeNVRAM(void);
void loadNVRAMDefaults(void);
void dumpNVRAM(void);
void clearNVRAM(void);
bool checkNVRAMIsValidData(void);

#endif

