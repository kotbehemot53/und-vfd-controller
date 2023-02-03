#ifndef PIUCNTVFD1_IV18ANIMATIONSTEPS_H
#define PIUCNTVFD1_IV18ANIMATIONSTEPS_H

#include "IV18Display.h"

class IV18AnimationSteps
{
private:
    IV18AnimationSteps() {}
public:
    static void doDigitStep(IV18Display* that, int sequenceNumber);
    static void prepareNextDigitStep(IV18Display* that, int sequenceNumber);
    static void statusOn(IV18Display* that, int sequenceNumber);
    static void statusOff(IV18Display* that, int sequenceNumber);
    static void grid9OffAndCooldown(IV18Display *that, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18ANIMATIONSTEPS_H
