#ifndef PIUCNTVFD1_DEVICEANIMATOR_H
#define PIUCNTVFD1_DEVICEANIMATOR_H

#include "DeviceAnimatorThread.h"

class DeviceAnimator
{
private:
    DeviceAnimatorThread* threads;
    int numberOfThreads;
    DeviceAnimatorStep** stepsMerged;
    int totalSteps = 0;
    unsigned long frameEndTimeUs = 0;

    static int compareMergedSteps(const void *cmp1, const void *cmp2);
    void initFrame();

public:
    // TODO: turn it into constructor?
    void setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet);

    void doFrame();
};

#endif //PIUCNTVFD1_DEVICEANIMATOR_H
