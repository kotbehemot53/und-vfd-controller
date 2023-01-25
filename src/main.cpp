// TODO: remove this include later?
#include <Arduino.h>

#include "../lib/IV18Display/IV18Display.h"
#include "../lib/I2CComms/I2CComms.h"
#include "../lib/IV18Display/IV18Animator.h"

// TODO: intro mode? as special IV18Display subclass? just a different animation routine and a switch?
// TODO: PWM brightness - special command + functions in IV18Display?

const byte I2C_ADDR = 0x5;
const unsigned long FRAME_DURATION_US = 10000;

IV18Animator* animator;

void setup()
{
    Serial.begin(115200);
    Serial.println("yo");

    Display.init();//FRAME_DURATION_US); //outputs, status diode (no hv_enable yet?, do on command only?)
    Comms.init(I2C_ADDR, Display); //wire begin, wire.onReceive TODO: make it minimal, just buffer data!

//    Animator.init(Display);
    animator = new IV18Animator(Display);

    // TODO: enable it by command, not here!
    delay(100);
    Display.on();

    // TODO: remove it when we have a nice heartbeat animated
//    Display.statusOn(&Display);
}

void loop()
{
    // TODO: animation with a fixed framerate, settable here
    //       handle timer overflow - when new value lower than previous, calc the true difference
    //             - nah fck it - it's every 70 mins

    // TODO: disable interrupts here? would omit packets?

//    Animator.doFrame();
    animator->doFrame();

    // TODO: debug, remove
//    delay(100);

    Comms.handleBufferedInput(); //if new_frame handleNewFrame...

//    Display.endFrame();
    // TODO: end frame? on Display?
}

