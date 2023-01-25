#include "DeviceAnimator.h"

#include <Arduino.h>

void DeviceAnimator::setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet)
{
    threads = threadsToSet;
    numberOfThreads = numberOfThreadsToSet;
}

void DeviceAnimator::doFrame()
{
    for (int i = 0; i < numberOfThreads; ++i) {
        // TODO: merge threads into 1, for now this just runs them in series!
        for (int j = 0; j < threads[i].numberOfSteps; ++j) {
            threads[i].steps[j].callback(threads[i].steps[j].callbackObjPtr);
            if (threads[i].steps[j].waitUs > 16000) {
                delay(threads[i].steps[j].waitUs / 1000);
            } else {
                delayMicroseconds(threads[i].steps[j].waitUs);
            }
        }
    }
}
