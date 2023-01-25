#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../Animator/DeviceAnimator.h"
#include "IV18Display.h"

class IV18Animator
{
private:
    DeviceAnimator animator;
    IV18Display currentDisplay;
    int currentFrame = 0;

    DeviceAnimatorThread* threads;
    DeviceAnimatorStep* heartbeatSteps;

public:
    IV18Animator(IV18Display display);

    void doFrame();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
