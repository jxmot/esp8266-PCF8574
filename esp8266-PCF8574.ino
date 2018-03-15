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
pcf8574 *p_pcf8574 = NULL;

// for interrupts that occur when an input changes
volatile bool intrFlag;
void ICACHE_RAM_ATTR intrHandler() 
{
    intrFlag = true;
}

/*
    Set up the application....
*/
void setup()
{
    WiFi.mode(WIFI_OFF);

    // force to true in case there is no interrupt handler,
    // then read will always return something.
    intrFlag = true;

    // Change the GPIOx labels to suit your ESP8266 platform
    p_pcf8574 = new pcf8574("GPIO0", "GPIO2");
    // NOTE : Regarding interrupts it is only necessary to pass the 
    // address to the interrupt handler one time. It is up to the
    // client to decide which devices to read when an interrupt occurs.
    // However it is probably best to read all inputs on all devices 
    // that are attached to the I2C bus when an interrupt happens.
    p_pcf8574->createDevice(0x20, intrHandler);
    // typically when creating a device without an interrupt handler
    // it would mean that this device is dedicated for output
    p_pcf8574->createDevice(0x21);

    while (!Serial);
    Serial.println();
    Serial.println();
    Serial.println("Starting.....");
    Serial.flush();
}

//////////////////////////////////////////////////////////////////////////////
/*
    Ever feel like your running in circles?
*/
void loop()
{
static int modcount = 0;

    // alternate between write tests...
    if(((modcount += 1) % 30) > 15) testCount(0x21, p_pcf8574);
    else testShift(0x21, p_pcf8574);
    Serial.flush();

    // the test will manage the state of the
    // interrupt flag
    intrFlag = testRead(0x20, p_pcf8574, intrFlag);
    Serial.flush();

    delay(250);
    yield();
}

