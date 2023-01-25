#include "DeviceAnimator.h"

#include <Arduino.h>

int DeviceAnimator::sortMergedSteps(const void *cmp1, const void *cmp2)
{
    auto a = (DeviceAnimatorStep **) cmp1;
    auto b = (DeviceAnimatorStep **) cmp2;

    // no subtraction is deliberate (the operands are unsigned longs and result is int)
    if ((*b)->timeWithinFrameUs > (*a)->timeWithinFrameUs) {
        return -1;
    }
    if ((*b)->timeWithinFrameUs < (*a)->timeWithinFrameUs) {
        return 1;
    }
    return 0;
}

void DeviceAnimator::setThreads(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet)
{
    int maxStepsInThread = 0;
    totalSteps = 0;

    // determine number of steps to preallocate merged steps pointers array
    for (int i = 0; i < numberOfThreadsToSet; ++i) {
        if (threadsToSet[i].numberOfSteps > maxStepsInThread) {
            maxStepsInThread = threadsToSet[i].numberOfSteps;
        }
        totalSteps += threadsToSet[i].numberOfSteps;
    }
    stepsMerged = new DeviceAnimatorStep*[totalSteps];

    // assign objective points in time to steps, initialize merged steps pointers array (unsorted yet!)
    int k = 0;
    for (int i = 0; i < numberOfThreadsToSet; ++i) {
        unsigned long currentPointInTimeInThreadUs = 0;
        for (int j = 0; j < threadsToSet[i].numberOfSteps; ++j) {
            threadsToSet[i].steps[j].timeWithinFrameUs = currentPointInTimeInThreadUs;
            currentPointInTimeInThreadUs += threadsToSet[i].steps[j].waitUs;
            stepsMerged[k] = &threadsToSet[i].steps[j];

            ++k;
        }
    }

    // sort the merged steps
    qsort(stepsMerged, totalSteps, sizeof(stepsMerged[0]), DeviceAnimator::sortMergedSteps);

    // assign times to next merged step
    unsigned long previousStepTimeWithinFrameUs = 0;
    for (int i = 0; i < totalSteps; ++i) {
        stepsMerged[i]->timeToNextMergedStepUs = stepsMerged[i]->timeWithinFrameUs - previousStepTimeWithinFrameUs;
        previousStepTimeWithinFrameUs = stepsMerged[i]->timeWithinFrameUs;
    }
}

void DeviceAnimator::doFrame()
{
    unsigned long preExecTimeUs;
    unsigned long postExecTimeUs;
    unsigned long execTimeDiffUs;
    unsigned long delayUs;

    for (int i = 0; i < totalSteps; ++i) {
        preExecTimeUs = micros();
        stepsMerged[i]->callback(stepsMerged[i]->callbackObjPtr);
        postExecTimeUs = micros();
        execTimeDiffUs = postExecTimeUs >= preExecTimeUs ? postExecTimeUs - preExecTimeUs : 0;

        // wait the appropriate amount of time for the next step (taking into account how much time it took to execute the step)
        // TODO: warn on timeToNextMergedStepUs < execTimeDiffUs (means too little intervals specified)
        delayUs = stepsMerged[i]->timeToNextMergedStepUs >= execTimeDiffUs ? stepsMerged[i]->timeToNextMergedStepUs - execTimeDiffUs : 0;
        if (delayUs > 16000) {
            delay(delayUs / 1000);
        } else {
            delayMicroseconds(delayUs);
        }
    }
}
