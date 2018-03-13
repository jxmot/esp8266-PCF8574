/*
*/
#include "src/pcf8574/pcf8574_esp.h"

typedef struct {
    uint8_t pin;
    String label;
} GPIOPIN;

class pcf8574 {

    public:
        pcf8574(String pinSDA, String pinSCL, uint8_t address, void (*ihandler)());
        uint8_t read8574(uint8_t pin = 99);
        void write8574(uint8_t val, uint8_t pin = 99);
        int lastError();
        bool checkForDevice(byte addr);

        PCF857x *p_pcf857x;
        byte I2Cerror;

    private:

        static const GPIOPIN gpiopins[];

        void initIntr(void (*ihandler)());

        bool initI2C(String sda, String scl, uint8_t address, void (*ihandler)());
        uint8_t getPort(String gpioX);
};

