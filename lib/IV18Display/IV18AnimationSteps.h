#ifndef PIUCNTVFD1_IV18ANIMATIONSTEPS_H
#define PIUCNTVFD1_IV18ANIMATIONSTEPS_H

#include "IV18Animator.h"

class IV18Animator;

/**
 * A collection of methods to be run as steps of the main animation by IV18Animator. Meant to be used as callbacks
 * in DeviceAnimationStep.
 */
class IV18AnimationSteps
{
private:
    IV18AnimationSteps() {}
public:
    static void doDigitStep(IV18Animator* animator, int sequenceNumber);
    static void prepareNextDigitStep(IV18Animator* animator, int sequenceNumber);
    static void statusOn(IV18Animator* animator, int sequenceNumber);
    static void statusOff(IV18Animator* animator, int sequenceNumber);
    static void grid9OffAndCooldown(IV18Animator* animator, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18ANIMATIONSTEPS_H
