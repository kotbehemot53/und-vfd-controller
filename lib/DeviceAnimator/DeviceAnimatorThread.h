#ifndef PIUCNTVFD1_DEVICEANIMATORTHREAD_H
#define PIUCNTVFD1_DEVICEANIMATORTHREAD_H

#include "DeviceAnimatorStep.h"

/**
 * A representation of a single animation thread
 */
class DeviceAnimatorThread
{
public:
    DeviceAnimatorStep* steps; // the collection of steps constituting the thread
    int numberOfSteps; // the number of steps

    DeviceAnimatorThread(DeviceAnimatorStep stepsToSet[], int numberOfStepsToSet) : steps(stepsToSet), numberOfSteps(numberOfStepsToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORTHREAD_H
