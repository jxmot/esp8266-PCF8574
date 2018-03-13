#include "pcf8574-test.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
/*
    Functions for testing read and write
*/
void testCount(pcf8574 *p_pcf8574)
{
static uint8_t pinval = 0;
int err = 0;

    Serial.println("testCount - val : " + String(byteToBin(pinval)));
    p_pcf8574->write8574(~pinval);
    pinval += 1;
    if(pinval >= 16) pinval = 0;
    if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("\ntestCount - ERROR = " + String(err));
}

void testShift(pcf8574 *p_pcf8574)
{
static uint8_t pinval = 1;
int err = 0;

    Serial.println("testShift - val : " + String(byteToBin(pinval)));
    p_pcf8574->write8574(~pinval);
    if((pinval <<= 1) >= 16) pinval = 1;
    if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("\ntestShift - ERROR = " + String(err));
}

bool testRead(pcf8574 *p_pcf8574, bool intr) 
{
bool iflag = false;
static uint8_t lastpinval = 0;
uint8_t pinval = 0;
int err = 0;

    // save the state of the interrupt flag and test for true
    if((iflag = intr) == true) {
        pinval = p_pcf8574->read8574();
        // don't clear the flag unless interrupts have been enabled
        if(p_pcf8574->intrenabled == true) iflag = false;
        if(lastpinval != pinval) {
            Serial.println("testRead - val : " + String(byteToBin(pinval)) + " last : " + String(byteToBin(lastpinval)));
            lastpinval = pinval;
        }
        if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("testRead - ERROR = " + String(err));
    }
    // return the interrupt flag state
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

