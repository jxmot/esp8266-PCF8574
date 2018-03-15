/*
    Test functions for the esp8266-PCF8574 sketch

    https://github.com/jxmot/esp8266-PCF8574
*/
#include "pcf8574-test.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
/*
    Count from 0 through 15 and output the bits to the PCF8574, increment the 
    count on each call to this function.
*/
void testCount(uint8_t address, pcf8574 *p_pcf8574)
{
static uint8_t pinval = 0;
int err = 0;

    Serial.println("testCount - val : " + String(byteToBin(pinval)));
    // invert the value before writing it, otherwise the lit LED would
    // be seen as inverted from the polarity of the corresponding bit
    p_pcf8574->write8574(address, ~pinval);
    pinval += 1;
    if(pinval >= 16) pinval = 0;
    if((err = p_pcf8574->lastError(address)) != PCF857x_OK) Serial.println("\ntestCount - ERROR = " + String(err));
}

/*
    Shift a single "on" bit from LSB to MSB
*/
void testShift(uint8_t address, pcf8574 *p_pcf8574)
{
static uint8_t pinval = 1;
int err = 0;

    Serial.println("testShift - val : " + String(byteToBin(pinval)));
    p_pcf8574->write8574(address, ~pinval);
    if((pinval <<= 1) >= 16) pinval = 1;
    if((err = p_pcf8574->lastError(address)) != PCF857x_OK) Serial.println("\ntestShift - ERROR = " + String(err));
}

/*
    Read the value (assumed to be all 8 bits) from the PCF8574 and compare the
    value to the last read, if different then report it. also manage the state
    of an interrupt flag.
*/
bool testRead(uint8_t address, pcf8574 *p_pcf8574, bool intr) 
{
bool iflag = false;
static uint8_t lastpinval = 0;
uint8_t pinval = 0;
int err = 0;

    // save the state of the interrupt flag and test for true
    if((iflag = intr) == true) {
        pinval = p_pcf8574->read8574(address);
        // don't clear the flag unless interrupts have been enabled
        if(p_pcf8574->isIntrEn(address) == true) iflag = false;
        if(lastpinval != pinval) {
            Serial.println("testRead - val : " + String(byteToBin(pinval)) + " last : " + String(byteToBin(lastpinval)));
            lastpinval = pinval;
        }
        if((err = p_pcf8574->lastError(address)) != PCF857x_OK) Serial.println("testRead - ERROR = " + String(err));
    }
    // return the next interrupt flag state
    return iflag;
}

/*
    Convert an 8 bit value into a string of 8 x 1's or 0's
*/
const char *byteToBin(uint8_t val)
{
static char bnumb[9];

    bnumb[0] = '\0';

    for(uint8_t bits = 128; bits > 0; bits >>= 1)
    {
        strcat(bnumb, ((val & bits) == bits) ? "1" : "0");
    }
    return bnumb;
}

#ifdef __cplusplus
}
#endif

