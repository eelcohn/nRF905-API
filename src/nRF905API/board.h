#ifndef __BOARD_H__
#define __BOARD_H__

#include "config.h"

/* Arduino ESP8266 boards */
#if defined(ARDUINO) && defined(ARDUINO_ARCH_ESP8266)
#include "src/hardware/esp8266.h"

/* Arduino ESP32 boards */
#elif defined(ARDUINO) && defined(ARDUINO_ARCH_ESP32)
#include "src/hardware/esp32.h"

/* Raspberry Pi BCM2835 */
#elif defined(RPI) && defined(CONFIG_ARCH_BCM2835)
#include "src/hardware/bcm2835.h"

/* Unsupported boards */
#else
#error This hardware/board is not supported.
#endif

#endif

