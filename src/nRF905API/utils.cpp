#include <Arduino.h>				// String
#include "utils.h"				// Function declarations

int hexstringToBin(const char * hexstring, uint8_t * bin, size_t max_size) {
	size_t		i,
			len;

	if (!isHex(hexstring))
		return -1;

	len = (strlen(hexstring) / 2);

	if (len > max_size)
		return -1;

	for (i = 0; i < len; i++) {
		if (isdigit(hexstring[i * 2]))
			bin[i] = ((hexstring[i * 2] - 0x30) << 4);
		else
			bin[i] = ((hexstring[i * 2] - 0x37) << 4);

		if (isdigit(hexstring[(i * 2) + 1]))
			bin[i] |= (hexstring[(i * 2) + 1] - 0x30);
		else
			bin[i] |= (hexstring[(i * 2) + 1] - 0x37);
	}

	return i;
}

void binToHexstring(const uint8_t *bin, char * hexstring, size_t size) {
	size_t		i;

	for (i = 0; i < size; i++) {
		hexstring[(i * 2)] = (((bin[i] & 0xF0) >> 4) + 0x30);
		if (hexstring[(i * 2)] > 0x39)
			hexstring[(i * 2)] += 7;
		hexstring[(i * 2) + 1] = ((bin[i] & 0x0F) + 0x30);
		if (hexstring[(i * 2) + 1] > 0x39)
			hexstring[(i * 2) + 1] += 7;
	}

	hexstring[(size * 2)] = '\0';
}

void uint8ToString(const uint8_t value, char * hexstring) {
	sprintf(hexstring, "%02X", value);
}
       
void uint32ToString(const uint32_t value, char * hexstring) {
	sprintf(hexstring, "%08X", value);
}
       
void serialPrintHex(const uint8_t * buffer, const size_t size) {
	size_t		n;

	for (n = 0; n < size; n++) {
		Serial.print("0x");
		Serial.print(buffer[n] < 16 ? "0" : "");
		Serial.print(buffer[n], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

bool isHex(const char * hexstring) {
	size_t		i,
			len;

	len = strlen(hexstring);

	if ((len == 0) || ((len % 2) != 0))
		return false;

	for (i = 0; i < len; i++) {
		if (!isHexadecimalDigit(hexstring[i]))
//		if ((hexstring[i] < '0') || ((hexstring[i] & 0xDF) > 'F'))
			return false;
//		if ((hexstring[i] > '9') && ((hexstring[i] & 0xDF) < 'A'))
//			return false;
	}

	return true;
}

bool isInt(const String & s) {
	char *		p;

	if (s.length() == 0 || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
		return false;

	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}

bool isFloat(const String & s) {
	char *		p;

	if (s.length() == 0 || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
		return false;

	strtof(s.c_str(), &p);

	return (*p == 0);
}

