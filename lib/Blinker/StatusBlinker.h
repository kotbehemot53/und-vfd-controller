#ifndef PIUCNTVFD1_STATUSBLINKER_H
#define PIUCNTVFD1_STATUSBLINKER_H

#include <Arduino.h>

class StatusBlinker {
private:
    static const uint8_t MUX_DA = 2;
    static const uint8_t MUX_CL = 3;
    static const uint8_t MUX_MR = 4;
    static const uint8_t HV_ENABLE = A0;
    static const uint8_t BLINK_OUTPUT = A3;

    static const unsigned long BLINK_DELAY = 100;

    bool initialised = false;

public:
//    Blinker();
    inline StatusBlinker() {}

    bool isInitialised();

    void init();
    void blinkFrame();
};

extern StatusBlinker Blinker;

#endif //PIUCNTVFD1_STATUSBLINKER_H
