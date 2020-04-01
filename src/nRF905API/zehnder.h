#ifndef __ZEHNDER_H__
#define __ZEHNDER_H__

#define ZEHNDER_FRAMESIZE		16
#define ZEHNDER_FRAME_SETPOWER		0x02
#define ZEHNDER_FRAME_SETTIMER		0x03
#define ZEHNDER_NETWORK_JOIN_REQUEST	0x04
#define ZEHNDER_FRAME_05		0x05
#define ZEHNDER_NETWORK_JOIN_OPEN	0x06
#define ZEHNDER_FRAME_0B		0x0B
#define ZEHNDER_NETWORK_JOIN_ACK	0x0C
#define ZEHNDER_NETWORK_JOIN_FINISH	0x0D

const uint32_t	network_link_id = 0xA55A5AA5;
struct Zehnder {
	uint8_t	main_unit_type;
	uint8_t	main_unit_id;
	uint8_t	device_type;
	uint8_t	device_id;
} zehnder;

typedef struct {
	uint8_t	rx_type	= 0x00;
	uint8_t	rx_id		= 0x00;
	uint8_t	tx_type	= 0x00;
	uint8_t	tx_id		= 0x00;
	uint8_t	ttl		= 0x00;
	uint8_t	frametype	= 0x00;
	uint8_t	parameters	= 0x00;
	struct {
		uint8_t	param1	= 0x00;
		uint8_t	param2	= 0x00;
		uint8_t	param3	= 0x00;
		uint8_t	param4	= 0x00;
	} address;
	uint8_t	param5		= 0x00;
	uint8_t	param6		= 0x00;
	uint8_t	param7		= 0x00;
	uint8_t	param8		= 0x00;
	uint8_t	param9		= 0x00;
} ZehnderFrame;

#endif

