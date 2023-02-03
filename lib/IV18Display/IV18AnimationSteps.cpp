#include "IV18AnimationSteps.h"

void IV18AnimationSteps::doGridStep(IV18Display* that, int sequenceNumber)
{
    that->prepareGridSegments(sequenceNumber);
}

void IV18AnimationSteps::prepareNextGridStep(IV18Display* that, int sequenceNumber)
{
    that->clearChar();
    that->multiplexViaShiftRegister(sequenceNumber == 0);
    that->Grid9OnWhenNeeded(sequenceNumber == 8);
}

void IV18AnimationSteps::statusOn(IV18Display* that, int sequenceNumber)
{
    that->statusOn();
}

void IV18AnimationSteps::statusOff(IV18Display* that, int sequenceNumber)
{
    that->statusOff();
}

void IV18AnimationSteps::grid9OffAndCooldown(IV18Display *that, int sequenceNumber)
{
    that->Grid9Off();
}