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
            stepsToSet), numberOfSteps(numberOfStepsToSet), isEnabled(isEnabledToSet) {
        for (int i = 0; i < numberOfSteps; ++i) {
            steps[i].parentThread = this;
        }
    };
};

#endif //PIUCNTVFD1_DEVICEANIMATORTHREAD_H
