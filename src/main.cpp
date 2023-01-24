// TODO: remove this include later?
#include <Arduino.h>

#include "../lib/VFDisplay/VFDisplay.h"
#include "../lib/I2CComms/I2CComms.h"

// TODO: intro mode? as special VFDisplay subclass?
//TODO: PWM brightness - special command + functions in VFDisplay?

const byte I2C_ADDR = 0x5;

void setup() {
    Display.init(); //outputs, status diode (no hv_enable yet?, do on command only?)
    Comms.init(I2C_ADDR, Display); //wire begin, wire.onReceive
}

void loop() {
    //TODO: begin frame? handle frame? on Display?

    Comms.handleBufferedInput(); //if new_frame handleNewFrame...

    //TODO: end frame? on Display?
}