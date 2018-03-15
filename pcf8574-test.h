/*
    Test functions for the esp8266-PCF8574 sketch

    https://github.com/jxmot/esp8266-PCF8574
*/
#include "pcf8574.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void testCount(uint8_t, pcf8574 *);
extern void testShift(uint8_t, pcf8574 *);
extern bool testRead(uint8_t, pcf8574 *, bool);
extern const char *byteToBin(uint8_t);

#ifdef __cplusplus
}
#endif
