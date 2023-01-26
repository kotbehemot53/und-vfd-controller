#ifndef PIUCNTVFD1_DEVICEANIMATORTHREAD_H
#define PIUCNTVFD1_DEVICEANIMATORTHREAD_H

#include "DeviceAnimatorStep.h"

class DeviceAnimatorThread
{
public:
    DeviceAnimatorStep *steps;
    int numberOfSteps;
    bool isEnabled;

    DeviceAnimatorThread(DeviceAnimatorStep stepsToSet[], int numberOfStepsToSet, bool isEnabledToSet = true) : steps(
            stepsToSet), numberOfSteps(numberOfStepsToSet), isEnabled(isEnabledToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORTHREAD_H
