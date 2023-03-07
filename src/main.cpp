#include <Arduino.h>

#include "../lib/IV18Display/IV18Display.h"
#include "../lib/I2CComms/I2CComms.h"
#include "../lib/IV18Display/IV18Animator.h"
#include "../lib/AnimatorFailureListener/IV18AnimatorSerialFailureListener.h"
#include "../lib/AnimatorFailureListener/IV18AnimatorLedFailureListener.h"

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

    // TODO: rly? a command for warning maybe?
    animator->doWarning(10);

    // run intro in the beginning
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_INTRO_ON);

    // debug
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_OUT | 10);
    I2CComms::addCommandToWriteBuffer(0);
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_TIME | 10);
    I2CComms::addCommandToWriteBuffer(255);
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    // run a frame of the basic animation
    animator->doFrame();

    // run high-level animation sequencing if applicable (e.g. if intro is on)
    animator->doCurrentSequencing();

// debug
    if (currentFrame == 499) {
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_INTRO_OFF);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_FINISH);
    }

    if (currentFrame == 1299) {
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 2);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 4);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 6);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 8);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_FINISH);
    }
    if (currentFrame == 1399) {
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 1);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 3);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 5);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 7);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_FADE_IN | 9);
        I2CComms::addCommandToWriteBuffer(255);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_FINISH);
    }

    ++currentFrame;
}

