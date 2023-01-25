#ifndef PIUCNTVFD1_DEVICEANIMATOR_H
#define PIUCNTVFD1_DEVICEANIMATOR_H

#include "DeviceAnimatorThread.h"

class DeviceAnimator
{
private:
    DeviceAnimatorStep** stepsMerged;
    int totalSteps;

    static int sortMergedSteps(const void *cmp1, const void *cmp2);

public:
    void setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet);

    void doFrame();
};

#endif //PIUCNTVFD1_DEVICEANIMATOR_H
