#include <Arduino.h>

#include "../lib/IV18Display/IV18Display.h"
#include "../lib/I2CComms/I2CComms.h"
#include "../lib/IV18Display/IV18Animator.h"
#include "../lib/AnimatorFailureListener/IV18AnimatorSerialFailureListener.h"

const byte I2C_ADDR = 0x5;

IV18Animator* animator;
AnimatorFailureListenerInterface* animatorFailureListener;

long currentFrame = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("yo");

    animator = new IV18Animator(&Display);
    I2CComms::init(I2C_ADDR);

    // TODO: use this for animation timing debug
//    animatorFailureListener = new IV18AnimatorSerialFailureListener(animator);

    // TODO: enable it by command, not here! Or disable by command?
//    delay(100);
    Display.on();

    // run intro in the beginning
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_INTRO_ON);
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    // run a frame of the basic animation
    animator->doFrame();

    // run high-level animation sequencing if applicable (e.g. if intro is on)
    animator->doCurrentSequencing();
}

