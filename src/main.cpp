// TODO: remove this include later?
#include <Arduino.h>

#include "../lib/IV18Display/IV18Display.h"
#include "../lib/I2CComms/I2CComms.h"
#include "../lib/IV18Display/IV18Animator.h"

// TODO: intro mode? as special IV18Display subclass? just a different animation routine and a switch?
// TODO: PWM brightness - special command + functions in IV18Display?
// TODO: comms

const byte I2C_ADDR = 0x5;
const unsigned long FRAME_DURATION_US = 10000;

IV18Animator* animator;

void setup()
{
    Serial.begin(115200);
    Serial.println("yo");

    // TODO: or constructor maybe?
    Comms.init(I2C_ADDR, Display); //wire begin, wire.onReceive TODO: make it minimal, just buffer data!

    animator = new IV18Animator(Display);

    // TODO: enable it by command, not here!
    delay(100);
    Display.on();
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    animator->doFrame();
    Comms.handleBufferedInput(); //if new_frame handleNewFrame...
}

