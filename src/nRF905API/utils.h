#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>				// String

void serialPrintHex(const uint8_t * buffer, const size_t size);
int hexstringToBin(const char * hexstring, uint8_t * bin, size_t max_size);
void binToHexstring(const uint8_t *bin, char * hexstring, size_t size);
void uint8ToString(const uint8_t value, char * hexstring);
void uint32ToString(const uint32_t value, char * hexstring);
bool isHex(const char * hexstring);
bool isInt(const String & s);
bool isFloat(const String & s);

#endif

