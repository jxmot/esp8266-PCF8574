/*
    Typical Usage :

        pcf8574 *p_pcf8574 = new pcf8574("GPIO0", "GPIO2", 0x20, intrHandler);

    Where :
        "GPIO0", "GPIO2" - are the desired GPIOx pins
        0x20 - the address of an I2C device
        intrHandler - pointer to a function such as :

            void ICACHE_RAM_ATTR intrHandler() 
            {
                // handle it, but don't take long and
                // be aware of which system calls do 
                // not work within an ISR. Just setting
                // a flag should do.
            }
        
*/
#include <Wire.h>
#include "pcf8574.h"

// table of GPIO labels to pin numbers
// To Do : obtain from a config file
const GPIOPIN pcf8574::gpiopins[] = {
    {16, "GPIO16"},
    { 5, "GPIO5"},
    { 4, "GPIO4"},
    { 0, "GPIO0"},
    { 2, "GPIO2"},
    {14, "GPIO14"},
    {12, "GPIO12"},
    {13, "GPIO13"},
    {99, "END"}
};

/*
    Class Constructor - 
*/
pcf8574::pcf8574(String pinSDA, String pinSCL, uint8_t address, void (*ihandler)())
{
    p_pcf857x = NULL;
    intrenabled = false;

    if(initI2C(pinSDA, pinSCL) == true) {
        if(createDevice(address)) initIntr(ihandler);
    }
}

/*
    Create a device but first determine if it attached
    to the I2C bus.
*/
bool pcf8574::createDevice(uint8_t address)
{
bool bRet = false;

    if(p_pcf857x == NULL) {
        if((bRet = checkForDevice(address)) == true) {
            p_pcf857x = new PCF857x(address, &Wire);
        }
    }
    return bRet;
}

/*
    Read 8 bits at a time
*/
uint8_t pcf8574::read8574(uint8_t pin) 
{
    if(p_pcf857x != NULL) {
        if(pin == 99) return p_pcf857x->read8();
        else return p_pcf857x->read(pin);
    }else return 0xFF;
}

/*
    Write up to 8 bits at a time
*/
void pcf8574::write8574(uint8_t val, uint8_t pin)
{
    if(p_pcf857x != NULL) {
        if(pin == 99) p_pcf857x->write8(val);
        else p_pcf857x->write(pin, val);
    }
}

/* 
    return the last know error in the PCF857x class
*/
int pcf8574::lastError() 
{
    if(p_pcf857x != NULL) return p_pcf857x->lastError();
    else return -1;
}

/*
    Set up interrupts appropriately based on whether or not a 
    pointer to an interrupt handler was passed in.
    
*/
void pcf8574::initIntr(void (*ihandler)())
{
uint8_t intrpin = 3;

    if(p_pcf857x != NULL) {
        if(ihandler == NULL) {
            p_pcf857x->begin();
            p_pcf857x->resetInterruptPin();
            intrenabled = false;
        } else {
            // set up the serial port so that we can use the GPIO 
            // pin for detecting an interrupt from the PCF8574
            Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);

            p_pcf857x->begin();
            pinMode(intrpin, FUNCTION_3);
            pinMode(intrpin, INPUT_PULLUP);
            p_pcf857x->resetInterruptPin();
            intrenabled = true;
            attachInterrupt(digitalPinToInterrupt(intrpin), ihandler, FALLING);
        }
    }
}

/*
    Initialize the I2C to use a specific pair of 
    GPIO pins
*/
bool pcf8574::initI2C(String sda, String scl)
{
bool bRet = false;

    uint8_t sdapin = getPort(sda);
    uint8_t sclpin = getPort(scl);

    if((sdapin != 99) && (sclpin != 99)) {
        Wire.begin(sdapin, sclpin);
        bRet = true;
    }
    return bRet;
}

/*
    Use the GPIO lookup table and find the GPIO pin
    label and return the corresponding pin number
*/
uint8_t pcf8574::getPort(String gpioX)
{
uint8_t gport;

    for(int ix = 0;;ix++) {
        if(gpiopins[ix].label == "END") {
            gport = 99;
            break;            
        }
        if(gpiopins[ix].label == gpioX) {
            gport = gpiopins[ix].pin;
            break;
        }
    }
    return gport;
}

/*
    See if a device is present by looking for its
    response.
*/
bool pcf8574::checkForDevice(byte addr)
{
bool bRet = false;

    Wire.beginTransmission(addr);
    I2Cerror = Wire.endTransmission();
    if(I2Cerror == 0) bRet = true;
    return bRet;
}
