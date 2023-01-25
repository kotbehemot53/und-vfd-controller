#ifndef PIUCNTVFD1_DEVICEANIMATORTHREAD_H
#define PIUCNTVFD1_DEVICEANIMATORTHREAD_H

#include "DeviceAnimatorStep.h"

class DeviceAnimatorThread
{
public:
    DeviceAnimatorStep* steps;
    int numberOfSteps;
    DeviceAnimatorThread(DeviceAnimatorStep stepsToSet[], int numberOfStepsToSet) : steps(stepsToSet), numberOfSteps(numberOfStepsToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORTHREAD_H
