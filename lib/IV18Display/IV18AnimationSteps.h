#ifndef PIUCNTVFD1_IV18ANIMATIONSTEPS_H
#define PIUCNTVFD1_IV18ANIMATIONSTEPS_H

#include "IV18Display.h"

class IV18AnimationSteps
{
private:
    IV18AnimationSteps() {}
public:
    static void doGridStep(IV18Display* that, int sequenceNumber);
    static void prepareNextGridStep(IV18Display* that, int sequenceNumber);
    static void statusOn(IV18Display* that, int sequenceNumber);
    static void statusOff(IV18Display* that, int sequenceNumber);
    static void noOp(IV18Display *that, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18ANIMATIONSTEPS_H
