#ifndef __FAN_H__
#define __FAN_H__

#include "src/hardware/nrf905.h"					// NRF905_REGISTER_COUNT

#define FAN_FRAMESIZE				16			// Each frame consists of 16 bytes
#define FAN_TX_FRAMES				4			// Retransmit every transmitted frame 4 times
#define FAN_TX_RETRIES				10			// Retry transmission 10 times if no reply is received
#define FAN_TTL				250			// 0xFA, default time-to-live for a frame
#define FAN_REPLY_TIMEOUT			500			// Wait 500ms for receiving a reply when doing a network scan

/* Fan device types */
enum {	FAN_TYPE_BROADCAST			= 0x00,		// Broadcast to all devices
	FAN_TYPE_MAIN_UNIT			= 0x01,		// Fans
	FAN_TYPE_REMOTE_CONTROL		= 0x03,		// Remote controls
	FAN_TYPE_CO2_SENSOR			= 0x18 };		// CO2 sensors

/* Fan commands */
enum {	FAN_FRAME_SETVOLTAGE			= 0x01,		// Set speed (voltage / percentage)
	FAN_FRAME_SETSPEED			= 0x02,		// Set speed (preset)
	FAN_FRAME_SETTIMER			= 0x03,		// Set speed with timer
	FAN_NETWORK_JOIN_REQUEST		= 0x04,
	FAN_FRAME_SETSPEED_REPLY		= 0x05,
	FAN_NETWORK_JOIN_OPEN			= 0x06,
	FAN_TYPE_FAN_SETTINGS			= 0x07,		// Current settings, sent by fan in reply to 0x01, 0x02, 0x10
	FAN_FRAME_0B				= 0x0B,
	FAN_FRAME_04				= 0x04,
	FAN_NETWORK_JOIN_ACK			= 0x0C,
//	FAN_NETWORK_JOIN_FINISH		= 0x0D,
	FAN_TYPE_QUERY_NETWORK			= 0x0D,
	FAN_TYPE_QUERY_DEVICE			= 0x10,
	FAN_FRAME_SETVOLTAGE_REPLY		= 0x1D };

/* Fan speed presets */
enum {	FAN_SPEED_AUTO				= 0x00,		// Off:      0% or  0.0 volt
	FAN_SPEED_LOW				= 0x01,		// Low:     30% or  3.0 volt
	FAN_SPEED_MEDIUM			= 0x02,		// Medium:  50% or  5.0 volt
	FAN_SPEED_HIGH				= 0x03,		// High:    90% or  9.0 volt
	FAN_SPEED_MAX				= 0x04 };		// Max:    100% or 10.0 volt

/* Internal result codes */
enum {	FAN_RESULT_SUCCESS			= 0x00,		// Success
	FAN_ERROR_NOT_FOUND,						// Remote device not found
	FAN_ERROR_NOT_COMPLETED,					// Join operation wasn't completed
	FAN_ERROR_TX_FAILED,						// Transmission failed
	FAN_ERROR_NO_REPLY,						// Remote device did not reply
	FAN_ERROR_NO_ACKNOWLEDGE,					// Remote device did not acknowledge
	FAN_ERROR_CONFIG_FAILED		= 0xFF };		// Failed to store nRF905 configuration

/* nRF905 configuration profiles */
enum {	FAN_PROFILE_NOT_CONFIGURED		= 0x00,		// nRF905 is not configured
	FAN_PROFILE_ZEHNDER,						// nRF905 configured for Zehnder fans
	FAN_PROFILE_BUVA,						// nRF905 configured for BUVA fans
	FAN_PROFILE_DEFAULT,						// nRF905 configured with factory default settings
	FAN_PROFILE_CUSTOM };						// nRF905 configured with custom settings

const uint32_t network_link_id PROGMEM		= 0xA55A5AA5;
const uint32_t network_default_id PROGMEM	= 0xE7E7E7E7;
const uint32_t FAN_JOIN_DEFAULT_TIMEOUT PROGMEM = 10000;
const uint8_t fan_profiles[][NRF905_REGISTER_COUNT]		= {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// FAN_PROFILE_NOT_CONFIGURED
	{0x76, 0x2E, 0x44, 0x10, 0x10, 0xA5, 0x5A, 0x5A, 0xA5, 0xDB},	// FAN_PROFILE_ZEHNDER
	{0x75, 0x2E, 0x44, 0x10, 0x10, 0xA5, 0x5A, 0x5A, 0xA5, 0xDB},	// FAN_PROFILE_BUVA
	{0x6C, 0x00, 0x44, 0x20, 0x20, 0xE7, 0xE7, 0xE7, 0xE7, 0xDB},	// FAN_PROFILE_DEFAULT
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	// FAN_PROFILE_CUSTOM
};

/* Zehnder/BUVA frame format */
typedef struct {
	uint8_t		rx_type	= 0x00;
	uint8_t		rx_id		= 0x00;
	uint8_t		tx_type	= 0x00;
	uint8_t		tx_id		= 0x00;
	uint8_t		ttl		= 0x00;
	uint8_t		frametype	= 0x00;
	uint8_t		parameters	= 0x00;
	struct {
		uint8_t	param1		= 0x00;
		uint8_t	param2		= 0x00;
		uint8_t	param3		= 0x00;
		uint8_t	param4		= 0x00;
	} address;
	uint8_t		param5		= 0x00;
	uint8_t		param6		= 0x00;
	uint8_t		param7		= 0x00;
	uint8_t		param8		= 0x00;
	uint8_t		param9		= 0x00;
} FanFrame;

class Fan {
	public:
		/* Functions */
				Fan(void);
				~Fan(void);
		size_t		discover(const uint8_t device_id, const uint32_t timeout);
		size_t		setSpeed(const uint8_t speed, const uint8_t timer);
		size_t		setVoltage(const uint8_t voltage);
		size_t		queryDevice(const uint8_t id, uint8_t *speed, uint8_t *voltage, uint8_t *timer);
		uint8_t	createDeviceID(void);
		size_t		networkScan(const uint32_t start, const uint32_t end);
		size_t		deviceScan(const uint32_t address);
		size_t		portScan(const uint32_t network, const uint8_t device_type, const uint8_t device_id, const uint8_t from_device_type, const uint8_t from_device_id);
		void		test_retransmit(const bool block);

	private:
		/* Functions */
		size_t		transmitData(size_t retries);

		/* Variables */
		uint8_t	known_devices[256];
};

#endif

