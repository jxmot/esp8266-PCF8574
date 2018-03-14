/*
    Test functions for the esp8266-PCF8574 sketch

    https://github.com/jxmot/esp8266-PCF8574
*/
#include "pcf8574.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void testCount(pcf8574 *);
extern void testShift(pcf8574 *);
extern bool testRead(pcf8574 *, bool);
extern const char *byteToBin(uint8_t);

#ifdef __cplusplus
}
#endif
