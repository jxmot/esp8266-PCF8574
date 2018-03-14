/* ************************************************************************ */
/*
    esp8266-PCF8574.ino - An ESP8266 application that communicates over I2C
    to operate a PCF8574 expanded I/O device.
*/

// required include files...
#include <Wire.h>
#include <ESP8266WiFi.h>

#include "pcf8574.h"
#include "pcf8574-test.h"

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
*/
void loop()
{
static int modcount = 0;

    // alternate between write tests...
    if(((modcount += 1) % 30) > 15) testCount(p_pcf8574_wr);
    else testShift(p_pcf8574_wr);

    // the test will manage the state of the
    // interrupt flag
    intrFlag = testRead(p_pcf8574_rd, intrFlag);

    delay(250);
    yield();
}

