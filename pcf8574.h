/*
    Encapsulates an object instantiated from the PCF857x class found in
    src/pcf8574/pcf8574_esp.h
*/
#pragma once

#include <map>
#include "src/pcf8574/pcf8574_esp.h"

using namespace std;

// For mapping GPIOx text labels <-> GPIOx pin number
typedef struct {
    uint8_t pin;
    String label;
} GPIOPIN;

/*
        
*/
class pcf8574 {

    public:
        pcf8574(String pinSDA, String pinSCL);
        bool createDevice(uint8_t address, void (*ihandler)() = NULL);

        uint8_t read8574(uint8_t address, uint8_t pin = 99);
        void write8574(uint8_t address, uint8_t val, uint8_t pin = 99);
        int lastError(uint8_t address);
        bool checkForDevice(byte addr);

        bool isIntrEn(uint8_t address);

        byte I2Cerror;
        bool intrenabled;
        std::map <uint8_t, PCF857x *> devmap;
        std::map <uint8_t, bool> intmap;

    private:

        static const GPIOPIN gpiopins[];
        PCF857x *p_pcf857x;
        bool i2cready;

        void initIntr(uint8_t address, void (*ihandler)());
        bool initI2C(String sda, String scl);
        uint8_t getGPIOpin(String gpioX);
};

