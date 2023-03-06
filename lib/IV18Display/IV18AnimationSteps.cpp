#include "IV18AnimationSteps.h"

void IV18AnimationSteps::doDigitStep(IV18Animator* animator, int sequenceNumber)
{
    animator->getDisplay()->prepareDigitSegments(sequenceNumber);
}

void IV18AnimationSteps::prepareNextDigitStep(IV18Animator* animator, int sequenceNumber)
{
    animator->getDisplay()->clearChar();
    animator->getDisplay()->multiplexViaShiftRegister(sequenceNumber == 0);
    animator->getDisplay()->Grid9OnWhenNeeded(sequenceNumber == 8);
}

void IV18AnimationSteps::statusOn(IV18Animator* animator, int sequenceNumber)
{
    animator->getDisplay()->statusOn();
}

void IV18AnimationSteps::statusOff(IV18Animator* animator, int sequenceNumber)
{
    animator->getDisplay()->statusOff();
}

void IV18AnimationSteps::grid9OffAndCooldown(IV18Animator *animator, int sequenceNumber)
{
    animator->getDisplay()->Grid9Off();
}