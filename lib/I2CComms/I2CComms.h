#ifndef PIUCNTVFD1_I2CCOMMS_H
#define PIUCNTVFD1_I2CCOMMS_H

#include <Arduino.h>

#include "../IV18Display/IV18Display.h"

class I2CComms
{

public:
    void init(byte addr, IV18Display display);

    void handleBufferedInput();
};

extern I2CComms Comms;

#endif //PIUCNTVFD1_I2CCOMMS_H
