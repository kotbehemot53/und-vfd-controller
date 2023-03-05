// TODO: remove this include later?
#include <Arduino.h>
//#include "avr8-stub.h" // avr-stub
//#include "app_api.h" // only needed with flash breakpoints

#include "../lib/IV18Display/IV18Display.h"
#include "../lib/I2CComms/I2CComms.h"
#include "../lib/IV18Display/IV18Animator.h"
#include "../lib/AnimatorFailureListener/IV18AnimatorSerialFailureListener.h"
#include "../lib/AnimatorFailureListener/IV18AnimatorLedFailureListener.h"

// TODO: intro mode? as special IV18Display subclass? just a different animation routine and a switch?
// TODO: PWM brightness - special command + functions in IV18Display?
// TODO: comms

const byte I2C_ADDR = 0x5;

IV18Animator* animator;
AnimatorFailureListenerInterface* animatorFailureListener;

unsigned long currentFrame = 0;
int animSteps[4] = {0, 0, 0, 0,};

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
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    animator->doFrame();

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
//    if (currentFrame % 500 == 499) {
//        if (Display.isOn()) {
//            I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_OFF);
//        } else {
//            I2CComms::addCommandToWriteBuffer(IV18I2CCommandExecutor::CMD_ON);
//        }
//    }

    if (currentFrame % 50 == 0) {
        for (int i = 1; i < 4; ++i) {
            animSteps[i] = animSteps[i - 1] - 2;
            if (animSteps[i] < 0) {
                animSteps[i] = animSteps[i] + 9;
            }
        }

        animator->setLampDigitAction(animSteps[0], IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[1], IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[2], IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[3], IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        ++animSteps[0];
        if (animSteps[0] > 8) {
            animSteps[0] = 0;
        }
    }

    ++currentFrame;
}

