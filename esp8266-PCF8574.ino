/* ************************************************************************ */
/*
    esp8266-PCF8574.ino - An ESP8266 application that communicates over I2Caddrs
    to operate a PCF8574 expanded I/O device.
*/

// required include files...
#include <Wire.h>
#include <ESP8266WiFi.h>

#define WRITE_OUTPUTS
#ifdef WRITE_OUTPUTS
#define WRITE_8BITS
// define one or the other, not both
//#define WRITE_COUNT
#define WRITE_SHIFT
#endif

#ifndef WRITE_OUTPUTS
#define READ_INPUTS
#define READ_8BITS
#define USE_INTRR
#endif 

#ifdef WRITE_OUTPUTS
// PCF857X code from -
// https://github.com/WereCatf/PCF8574_ESP
#include "src/pcf8574/pcf8574_esp.h"

PCF857x pcf8574(0x21, &Wire);
#endif

void write8574(uint8_t, uint8_t);

#ifdef READ_INPUTS
// PCF857X code from -
// https://github.com/WereCatf/PCF8574_ESP
#include "src/pcf8574/pcf8574_esp.h"

PCF857x pcf8574(0x20, &Wire);

uint8_t read8574(uint8_t);

#endif

#ifdef USE_INTRR
// GPIO3
uint8_t intrpin = 3;
#endif

typedef struct {
    uint8_t pin;
    String label;
} GPIOPIN;

const GPIOPIN gpiopins[] = {
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

///////////////////////////////////
uint8_t I2Cports[2];
byte I2Cerror;

#ifdef READ_INPUTS
const uint8_t MAX_DEVPINS = 8;
#endif

#ifdef USE_INTRR
volatile bool intrFlag = false;

void ICACHE_RAM_ATTR intrHandler() 
{
  intrFlag = true;
}
#endif

void setup()
{
    WiFi.mode(WIFI_OFF);

#ifdef USE_INTRR
    Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);
#else
    Serial.begin(115200);
#endif

    while (!Serial);
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("Looking for devices....");
    if(initI2C() == true) {
#ifdef READ_INPUTS
        if(I2Caddrs() == true) initIntr();
#else
        I2Caddrs();
#endif
    } else Serial.println("FAIL - " + String(I2Cports[0]) + "   " + String(I2Cports[1]));
}

void loop()
{

#ifdef WRITE_OUTPUTS
#ifdef WRITE_8BITS
#ifdef WRITE_COUNT
static uint8_t pinval = 0;

    Serial.println("\write8574 - val : " + String(byte_to_binary(pinval)));
    write8574(99, ~pinval);
    pinval += 1;
    if(pinval >= 16) pinval = 0;
    delay(1000);
#else
#ifdef WRITE_SHIFT
static uint8_t pinval = 1;

    Serial.println("\write8574 - val : " + String(byte_to_binary(pinval)));
    write8574(99, ~pinval);
    pinval <<= 1;
    if(pinval >= 16) pinval = 1;
    delay(1000);
#endif
#endif  // WRITE_COUNT

#else   // WRITE_8BITS
static uint8_t pinid = 0;
#endif  // WRITE_8BITS
#endif

#ifdef READ_INPUTS
static uint8_t pinid = 0;
uint8_t pinval = 0;
int err = 0;

#ifdef USE_INTRR
    if(intrFlag == true) {
        pinval = read8574(99);
        Serial.println("read8574(intr) - val = " + String(byte_to_binary(pinval)));
        intrFlag = false;
    }
#else   // USE_INTRR
#ifdef READ_8BITS
static uint8_t lastpinval = 0;

    pinval = read8574(99);

    if(lastpinval != pinval) {
        Serial.println("\nread8574 - val : " + String(byte_to_binary(pinval)) + " last : " + String(byte_to_binary(lastpinval)));
        lastpinval = pinval;
    }
#else
    pinval = read8574(pinid);

    Serial.println("read8574 - pin = " + String(pinid) + "   val = " + String(pinval));
    pinid += 1;

    if(pinid >= MAX_DEVPINS) {
        Serial.println("==================================");
        pinid = 0;
    }
#endif  // READ_8BITS
#endif  // USE_INTRR
    if((err = lastError()) != PCF857x_OK) Serial.println("read8574 - ERROR = " + String(err));
    delay(250);
#endif  // READ_INPUTS
    yield();
}

///////////////////////////////////
#ifdef READ_INPUTS
void initIntr()
{
    pcf8574.begin();
#ifdef USE_INTRR
    pinMode(intrpin, FUNCTION_3);
    pinMode(intrpin, INPUT_PULLUP);
    pcf8574.resetInterruptPin();
    attachInterrupt(digitalPinToInterrupt(intrpin), intrHandler, FALLING);
#else
    pcf8574.resetInterruptPin();
#endif
}

uint8_t read8574(uint8_t pin = 99) 
{
#ifdef USE_INTRR
    return pcf8574.read8();
#else
#ifdef READ_8BITS
    return pcf8574.read8();
#else
    if(pin != 99) return pcf8574.read(pin);
    else return 99;
#endif
#endif // USE_INTRR
}
#endif  // READ_INPUTS

void write8574(uint8_t pin, uint8_t val)
{
#ifdef WRITE_8BITS
    pcf8574.write8(val);
#else
    pcf8574.write(pin, val);
#endif
}

int lastError() 
{
    return pcf8574.lastError();
}

const char *byte_to_binary(uint8_t val)
{
static char bnumb[9];

    bnumb[0] = '\0';

    for(uint8_t bits = 128; bits > 0; bits >>= 1)
    {
        strcat(bnumb, ((val & bits) == bits) ? "1" : "0");
    }
    return bnumb;
}
///////////////////////////////////
bool initI2C()
{
bool bRet = false;

    if((bRet = getPorts("GPIO0", "GPIO2")) == true) {
        Wire.begin(I2Cports[0], I2Cports[1]);
        bRet = true;
    }
    return bRet;
}

bool getPorts(String gpio1, String gpio2)
{
bool bRet = false;

    I2Cports[0] = I2Cports[1] = 99;
    assignPort(gpio1, 0);
    assignPort(gpio2, 1);

    if((I2Cports[0] != 99) && (I2Cports[1] != 99)) {
        bRet = true;
    }
    return bRet;
}

void assignPort(String gpioX, int portsIdx)
{
    for(int ix = 0;;ix++) {
        if(gpiopins[ix].label == "END") {
            I2Cports[portsIdx] = 99;
            break;            
        }
        if(gpiopins[ix].label == gpioX) {
            I2Cports[portsIdx] = gpiopins[ix].pin;
            break;
        }
    }
}

///////////////////////
bool I2Caddrs() 
{
byte addr;
int devqty = 0;
bool bRet = false;
String found = "";

    for(addr = 1; addr < 127; addr++) {
        if(checkForDevice(addr)) {
            Serial.println("device found - addr : " + addrhex(addr));
            found = found + ((devqty > 0) ? "," : "") + addrhex(addr);
            devqty++;
        } else if(I2Cerror != 2) Serial.println("device ERROR - addr : " + addrhex(addr) + ", code : " + String(I2Cerror));
    }
    if (devqty == 0) Serial.println("no devices found");
    else {
        bRet = true;
        Serial.println("qty : " + String(devqty) + ", addrs : " + found);
    }
    return bRet;
}

bool checkForDevice(byte addr)
{
bool bRet = false;

    Wire.beginTransmission(addr);
    I2Cerror = Wire.endTransmission();
    if(I2Cerror == 0) {
        bRet = true;
    }
    return bRet;
}

String addrhex(byte addr)
{
char buf[17];

    sprintf(buf,"0x%02X", addr);
    return String(buf);
}

