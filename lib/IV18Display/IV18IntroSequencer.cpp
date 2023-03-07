#include "IV18IntroSequencer.h"

unsigned long IV18IntroSequencer::currentFrame = 0;
int IV18IntroSequencer::animSteps[4] = {0, 0, 0, 0,};

void IV18IntroSequencer::doIntroSequencing(IV18Animator *animator)
{
    if (currentFrame % 50 == 0) {
        for (int i = 1; i < 4; ++i) {
            animSteps[i] = animSteps[i - 1] - 2;
            if (animSteps[i] < 0) {
                animSteps[i] = animSteps[i] + 9;
            }
        }

        animator->setLampDigitAction(animSteps[0], IV18Animator::LAMP_MODE_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[1], IV18Animator::LAMP_MODE_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[2], IV18Animator::LAMP_MODE_DIGIT_OUT, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        animator->setLampDigitAction(animSteps[3], IV18Animator::LAMP_MODE_DIGIT_IN, IV18Animator::LAMP_DIGIT_MAX_DUTY_US,
                                     IV18Animator::LAMP_DIGIT_CUTOUT_DUTY_US + 1);
        ++animSteps[0];
        if (animSteps[0] > 8) {
            animSteps[0] = 0;
        }
    }

    ++currentFrame;
}
