#ifndef PIUCNTVFD1_DEVICEANIMATOR_H
#define PIUCNTVFD1_DEVICEANIMATOR_H

#include "DeviceAnimatorThread.h"

class DeviceAnimator
{
private:
    DeviceAnimatorThread* threads;
    int numberOfThreads;

public:
    void setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet);

    void doFrame();
};

#endif //PIUCNTVFD1_DEVICEANIMATOR_H
