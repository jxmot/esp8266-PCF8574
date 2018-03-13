/* ************************************************************************ */
/*
    esp8266-PCF8574.ino - An ESP8266 application that communicates over I2C
    to operate a PCF8574 expanded I/O device.
*/

// required include files...
#include <Wire.h>
#include <ESP8266WiFi.h>

#include "pcf8574.h"

// pointers to specific I2C devices, in this sketch one
// is for reading switches and the other one toggles LEDs
pcf8574 *p_pcf8574_rd = NULL;
pcf8574 *p_pcf8574_wr = NULL;

// for interrupts that occur when an input changes
volatile bool intrFlag;
void ICACHE_RAM_ATTR intrHandler() 
{
    intrFlag = true;
}

/*
*/
void setup()
{
    WiFi.mode(WIFI_OFF);

    // force to true in case there is no interrupt handler,
    // then read will always return something.
    intrFlag = true;

    p_pcf8574_rd = new pcf8574("GPIO0", "GPIO2", 0x20, intrHandler);
    p_pcf8574_wr = new pcf8574("GPIO0", "GPIO2", 0x21);

    while (!Serial);
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("Starting.....");
}

//////////////////////////////////////////////////////////////////////////////
/*
    Functions for testing read and write
*/
void testCount()
{
static uint8_t pinval = 0;
int err = 0;

    Serial.println("testCount - val : " + String(byteToBin(pinval)));
    p_pcf8574_wr->write8574(~pinval);
    pinval += 1;
    if(pinval >= 16) pinval = 0;
    if((err = p_pcf8574_wr->lastError()) != PCF857x_OK) Serial.println("\ntestCount - ERROR = " + String(err));
}

void testShift()
{
static uint8_t pinval = 1;
int err = 0;

    Serial.println("testShift - val : " + String(byteToBin(pinval)));
    p_pcf8574_wr->write8574(~pinval);
    if((pinval <<= 1) >= 16) pinval = 1;
    if((err = p_pcf8574_wr->lastError()) != PCF857x_OK) Serial.println("\ntestShift - ERROR = " + String(err));
}

void testRead() 
{
static uint8_t lastpinval = 0;
uint8_t pinval = 0;
int err = 0;

    if(intrFlag == true) {
        pinval = p_pcf8574_rd->read8574();
        // don't clear the flag unless interrupts have been enabled
        if(p_pcf8574_rd->intrenabled == true) intrFlag = false;
        if(lastpinval != pinval) {
            Serial.println("testRead - val : " + String(byteToBin(pinval)) + " last : " + String(byteToBin(lastpinval)));
            lastpinval = pinval;
        }
        if((err = p_pcf8574_rd->lastError()) != PCF857x_OK) Serial.println("testRead - ERROR = " + String(err));
    }
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

//////////////////////////////////////////////////////////////////////////////
/*
*/
int modcount = 0;

void loop()
{
    // alternate between write tests...
    if(((modcount += 1) % 30) > 15) testCount();
    else testShift();

    testRead();

    delay(250);
    yield();
}

