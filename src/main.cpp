// TODO: remove this include later?
#include <Arduino.h>
//#include "avr8-stub.h" // avr-stub
//#include "app_api.h" // only needed with flash breakpoints

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

//    debug_init(); //avr-stub

    animator = new IV18Animator(&Display);
    I2CComms::init(I2C_ADDR);

    // TODO: use these for animation timing debug
//    animatorFailureListener = new IV18AnimatorLedFailureListener(animator);
//    animatorFailureListener = new IV18AnimatorSerialFailureListener(animator);

    // TODO: enable it by command, not here!
    delay(100);
    Display.on();

    // TODO: debug, set via commands later
//    byte testBytes[9] = {0b00000000, 0b00010000, 0b00000000, 0b00010000, 0b00000000, 0b00010000,
//                      0b00000000, 0b00010000, 0b00000000};
//    Display.setChars(" czlonek ");
//    Display.setMode(IV18Display::MODE_BYTES);
//    Display.setBytes(testBytes);
    animator->doWarning(10);
//    for (int i = 0; i < IV18Display::DIGIT_STEPS_COUNT; ++i) {
//        animator->setCurrentLampDigitDutyValue(i, IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
//    }

    // TODO: command buffering debug - remove later
//    I2CComms::addCommandToWriteBuffer(0x00);
//    I2CComms::addCommandToWriteBuffer(0x01);
//    I2CComms::addCommandToWriteBuffer(0x02);
//
//    I2CComms::resetBuffers();
//    while (I2CComms::getReadBufferRemainingCommandCount()) {
//        Serial.println(I2CComms::getCommandFromReadBuffer(), HEX);
//    }
//
//    I2CComms::addCommandToWriteBuffer(0x03);
//    I2CComms::addCommandToWriteBuffer(0x04);
//
//    Serial.println("Premature ejac");
//    while (I2CComms::getReadBufferRemainingCommandCount()) {
//        Serial.println(I2CComms::getCommandFromReadBuffer(), HEX);
//    }
//
//    I2CComms::addCommandToWriteBuffer(0x05);
//    I2CComms::addCommandToWriteBuffer(0x06);
//
//    Serial.println("Real ejac");
//
//    I2CComms::resetBuffers();
//    while (I2CComms::getReadBufferRemainingCommandCount()) {
//        Serial.println(I2CComms::getCommandFromReadBuffer(), HEX);
//    }

// debug
//    Display.off();
//    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_OFF);

    // run intro in the beginning
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_INTRO_ON);

    // debug
//    Serial.println(IV18Animator::convertDutyCycle(0));
//    Serial.println(IV18Animator::convertDutyCycle(100));
//    Serial.println(IV18Animator::convertDutyCycle(128));
//    Serial.println(IV18Animator::convertDutyCycle(255));

    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_CHAR | 2);
    I2CComms::addCommandToWriteBuffer('a');
    I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_DIGIT_CHAR | 4);
    I2CComms::addCommandToWriteBuffer('c');
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    // run a frame of the basic animation
    animator->doFrame();

    // run high-level animation sequencing if applicable (e.g. if intro is on)
    animator->doCurrentSequencing();

//    if (currentFrame % 100 == 0 && currentFrame < 900) {
//        short digitNumber = currentFrame / 100;
//        animator->setLampDigitAction(digitNumber, IV18Animator::LAMP_DIGIT_OUT);
//    }
//
//    if (currentFrame % 100 == 0 && currentFrame >= 1800 && currentFrame < 2700) {
//        short digitNumber = 26 - (currentFrame / 100);
//        animator->setLampDigitAction(digitNumber, IV18Animator::LAMP_DIGIT_IN);
//    }

// debug
    if (currentFrame % 500 == 499) {
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_INTRO_OFF);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_CLEAR);
        I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_MULTI_FINISH);

//        if (Display.isOn()) {
//            I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_OFF);
//        } else {
//            I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_ON);
//        }
    }

    ++currentFrame;
}

