#ifndef __BOARD_H__
#define __BOARD_H__

#include "config.h"

#ifdef ARDUINO
#ifdef ARDUINO_ARCH_ESP32
#include "esp32.h"
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include "esp8266.h"
#endif
#endif

extern Board * board;

#endif

