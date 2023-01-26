#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18Display.h"

class IV18Animator
{
private:
    static const short FRAMES_PER_CYCLE = 150;
    static const unsigned long FRAME_LENGTH_US = 9000; // 1/100 s
    static const unsigned long DIMMEST_LENGTH_US = 500;

    DeviceAnimator animator;
    int currentFrame = 0;

    DeviceAnimatorThread* threads;
    DeviceAnimatorThread* heartbeatThread;
    DeviceAnimatorThread* lampGridThread;

    void animateHeartbeat();

public:
    IV18Animator(IV18Display &display);

    void doFrame();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
