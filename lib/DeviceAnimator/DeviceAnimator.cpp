#include "DeviceAnimator.h"

#include <Arduino.h>

int DeviceAnimator::compareMergedSteps(const void *cmp1, const void *cmp2)
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
    threads = threadsToSet;
    numberOfThreads = numberOfThreadsToSet;
    frameEndTimeUs = 0;
    totalSteps = 0;

    // determine number of steps to preallocate merged steps pointers array
    for (int i = 0; i < numberOfThreads; ++i) {
        totalSteps += threads[i].numberOfSteps;
    }
    stepsMerged = new DeviceAnimatorStep*[totalSteps];
    int k = 0;
    for (int i = 0; i < numberOfThreads; ++i) {
        for (int j = 0; j < threads[i].numberOfSteps; ++j) {
            stepsMerged[k] = &threads[i].steps[j];
            // now they are set arbitrarily by user
//            stepsMerged[k]->sequenceNumber = j;
            ++k;
        }
    }
}

void DeviceAnimator::initFrame()
{
    // assign objective points in time to steps, initialize merged steps pointers array (unsorted yet!)
    for (int i = 0; i < numberOfThreads; ++i) {
        unsigned long currentPointInTimeInThreadUs = 0;
        for (int j = 0; j < threads[i].numberOfSteps; ++j) {
            threads[i].steps[j].timeWithinFrameUs = currentPointInTimeInThreadUs;
            currentPointInTimeInThreadUs += threads[i].steps[j].waitUs;

            if (currentPointInTimeInThreadUs > frameEndTimeUs) {
                frameEndTimeUs = currentPointInTimeInThreadUs;
            }
        }
    }

    // sort the merged steps
    qsort(stepsMerged, totalSteps, sizeof(stepsMerged[0]), compareMergedSteps);

    // assign times to next merged step
    unsigned long previousStepTimeWithinFrameUs = 0;
    for (int i = 1; i < totalSteps; ++i) {
        stepsMerged[i - 1]->timeToNextMergedStepUs = stepsMerged[i]->timeWithinFrameUs - previousStepTimeWithinFrameUs;
        previousStepTimeWithinFrameUs = stepsMerged[i]->timeWithinFrameUs;
    }
    // for the last step, set time to next step, which actually is the end of frame
    stepsMerged[totalSteps - 1]->timeToNextMergedStepUs = frameEndTimeUs - stepsMerged[totalSteps - 1]->timeWithinFrameUs;
}

void DeviceAnimator::doFrame()
{
    unsigned long preExecTimeUs;
    unsigned long postExecTimeUs;
    unsigned long execTimeDiffUs;
    unsigned long delayUs;

    // happens every frame because steps contents may change between frames; takes about 40 us
    initFrame();

    for (int i = 0; i < totalSteps; ++i) {
        preExecTimeUs = micros();
        stepsMerged[i]->callback(stepsMerged[i]->devicePtr, stepsMerged[i]->sequenceNumber);
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