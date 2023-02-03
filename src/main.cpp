// TODO: remove this include later?
#include <Arduino.h>

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
bool on = true;

void setup()
{
    Serial.begin(115200);
    Serial.println("yo");

    // TODO: or constructor maybe?
    // TODO: make it minimal, just buffer data!
    Comms.init(I2C_ADDR, Display); //wire begin, wire.onReceive

    animator = new IV18Animator(Display);

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
    // TODO: why isn't minus dimming?
//    for (int i = 0; i < IV18Display::DIGIT_STEPS_COUNT; ++i) {
//        animator->setCurrentLampDigitDutyValue(i, 0);
//    }
}

void loop()
{
    // TODO: disable interrupts here? would omit packets?

    animator->doFrame();
    Comms.handleBufferedInput(); //if new_frame handleNewFrame...

//    if (currentFrame % 100 == 0 && currentFrame < 900) {
//        short digitNumber = currentFrame / 100;
//        animator->setLampDigitAction(digitNumber, IV18Animator::LAMP_DIGIT_OUT);
//    }
//
//    if (currentFrame % 100 == 0 && currentFrame >= 1800 && currentFrame < 2700) {
//        short digitNumber = 26 - (currentFrame / 100);
//        animator->setLampDigitAction(digitNumber, IV18Animator::LAMP_DIGIT_IN);
//    }

    if (currentFrame % 200 == 0) {
        if (on) {
            animator->setLampDigitAction(8, IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                         IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US); // + 1);
//            animator->setLampDigitAction(6, IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US);
            animator->setLampDigitAction(4, IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                         IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US); // + 1);
//            animator->setLampDigitAction(2, IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US);
//            animator->setLampDigitAction(0, IV18Animator::LAMP_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US, IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        } else {
            animator->setLampDigitAction(8, IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                         IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US); // + 1);
//            animator->setLampDigitAction(6, IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US);
            animator->setLampDigitAction(4, IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                         IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US); // + 1);
//            animator->setLampDigitAction(2, IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US);
//            animator->setLampDigitAction(0, IV18Animator::LAMP_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US, IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        }
        on = !on;
    }

//    if (currentFrame < 2700) {
        ++currentFrame;
//    }
}

