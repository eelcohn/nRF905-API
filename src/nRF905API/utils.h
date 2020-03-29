#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>

bool validHex(const char * hexstring);
void serialPrintHex(const uint8_t * buffer, const size_t size);
int hexstringToBin(const char * hexstring, uint8_t * bin, size_t max_size);
void binToHexstring(const uint8_t *bin, char * hexstring, size_t size);

#endif

