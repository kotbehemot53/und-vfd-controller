#ifndef PIUCNTVFD1_DEVICEANIMATOR_H
#define PIUCNTVFD1_DEVICEANIMATOR_H

#include "DeviceAnimatorThread.h"
#include "../AnimatorFailureListener/AnimatorFailureListenerInterface.h"

class DeviceAnimator
{
private:
    // overhead of all calculations after calculating the exec time but before actually doing the final wait/delay
    // determined experimentally
    static const int EXEC_TIME_OVERHEAD_US = 8;

    DeviceAnimatorThread* threads;
    int numberOfThreads = 0;
    DeviceAnimatorStep** stepsMerged;
    int totalSteps = 0;
    unsigned long frameEndTimeUs = 0;
    unsigned long lastFrameCummulativeUndertime = 0;

    AnimatorFailureListenerInterface* failureListener;
    bool hasFailureListener = false;

    static int compareMergedSteps(const void *cmp1, const void *cmp2);
    void initFrame();

public:
    // TODO: turn these into constructor?
    void setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet);
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    void doFrame();
};

#endif //PIUCNTVFD1_DEVICEANIMATOR_H
