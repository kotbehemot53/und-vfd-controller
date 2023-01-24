#ifndef PIUCNTVFD1_I2CCOMMS_H
#define PIUCNTVFD1_I2CCOMMS_H

#include <Arduino.h>

#include "../VFDisplay/VFDisplay.h"

class I2CComms {

public:
    void init(byte addr, VFDisplay display);
    void handleBufferedInput();
};

extern I2CComms Comms;

#endif //PIUCNTVFD1_I2CCOMMS_H
