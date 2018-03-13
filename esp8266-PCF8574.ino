/* ************************************************************************ */
/*
    esp8266-PCF8574.ino - An ESP8266 application that communicates over I2Caddrs
    to operate a PCF8574 expanded I/O device.
*/

// required include files...
#include <Wire.h>
#include <ESP8266WiFi.h>

#include "pcf8574.h"

pcf8574 *p_pcf8574 = NULL;

//
volatile bool intrFlag;
//ICACHE_RAM_ATTR
void ICACHE_RAM_ATTR intrHandler() 
{
    intrFlag = true;
}

void setup()
{
    WiFi.mode(WIFI_OFF);

    p_pcf8574 = new pcf8574("GPIO0", "GPIO2", 0x21, intrHandler);

    while (!Serial);
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("Starting.....");
}

void testCount()
{
static uint8_t pinval = 0;
int err = 0;

    Serial.println("testCount - val : " + String(byteToBin(pinval)));
    p_pcf8574->write8574(~pinval);
    pinval += 1;
    if(pinval >= 16) pinval = 0;
    if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("testCount - ERROR = " + String(err));
}

void testShift()
{
static uint8_t pinval = 1;
int err = 0;

    Serial.println("\testShift - val : " + String(byteToBin(pinval)));
    p_pcf8574->write8574(~pinval);
    pinval <<= 1;
    if(pinval >= 16) pinval = 1;
    if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("testShift - ERROR = " + String(err));
}

void testRead() 
{
static uint8_t lastpinval = 0;
uint8_t pinval = 0;
int err = 0;

    pinval = p_pcf8574->read8574();

    if(lastpinval != pinval) {
        Serial.println("\testRead - val : " + String(byteToBin(pinval)) + " last : " + String(byteToBin(lastpinval)));
        lastpinval = pinval;
    }
    if((err = p_pcf8574->lastError()) != PCF857x_OK) Serial.println("testRead - ERROR = " + String(err));
}


void loop()
{
    testCount();
    delay(250);
    yield();
}

///////////////////////////////////
///////////////////////////////////
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

///////////////////////
bool I2Caddrs() 
{
byte addr;
int devqty = 0;
bool bRet = false;
String found = "";

    for(addr = 1; addr < 127; addr++) {
        if(p_pcf8574->checkForDevice(addr)) {
            Serial.println("device found - addr : " + addrhex(addr));
            found = found + ((devqty > 0) ? "," : "") + addrhex(addr);
            devqty++;
        } else if(p_pcf8574->I2Cerror != 2) Serial.println("device ERROR - addr : " + addrhex(addr) + ", code : " + String(p_pcf8574->I2Cerror));
    }
    if (devqty == 0) Serial.println("no devices found");
    else {
        bRet = true;
        Serial.println("qty : " + String(devqty) + ", addrs : " + found);
    }
    return bRet;
}

String addrhex(byte addr)
{
char buf[17];

    sprintf(buf,"0x%02X", addr);
    return String(buf);
}

