#include "DeviceAnimator.h"

#include <Arduino.h>

DeviceAnimator::DeviceAnimator(DeviceAnimatorThread *threadsToSet, int numberOfThreadsToSet)
{
    threads = threadsToSet;
    numberOfThreads = numberOfThreadsToSet;
    totalSteps = 0;

    // determine number of steps to preallocate merged steps pointers array
    for (int i = 0; i < numberOfThreads; ++i) {
        totalSteps += threads[i].numberOfSteps;
    }
    stepsMerged = new DeviceAnimatorStep*[totalSteps];
    int k = 0;

    // prepare the merged array of steps; it still needs to be sorted before each frame.
    for (int i = 0; i < numberOfThreads; ++i) {
        for (int j = 0; j < threads[i].numberOfSteps; ++j) {
            stepsMerged[k] = &threads[i].steps[j];
            // now they are set arbitrarily by user
//            stepsMerged[k]->sequenceNumber = j;
            ++k;
        }
    }
}

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

void DeviceAnimator::setFailureListener(AnimatorFailureListenerInterface* failureListenerToSet)
{
    this->failureListener = failureListenerToSet;
    hasFailureListener = true;
}

void DeviceAnimator::initFrame()
{
    frameEndTimeUs = 0;

    // assign objective points in time to steps, initialize merged steps pointers array (unsorted yet!)
    for (int i = 0; i < numberOfThreads; ++i) {
        unsigned long currentPointInTimeInThreadUs = 0;
        for (int j = 0; j < threads[i].numberOfSteps; ++j) {
            threads[i].steps[j].timeWithinFrameUs = currentPointInTimeInThreadUs;
            currentPointInTimeInThreadUs += threads[i].steps[j].runningTimeUs;

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
    // for the last step, set time left to the next step, which actually is the end of frame
    // include last cummulative undertime in frameEndTimeUs to have a real endtime and not finish the last step too early
    stepsMerged[totalSteps - 1]->timeToNextMergedStepUs = frameEndTimeUs + lastFrameCummulativeUndertime - stepsMerged[totalSteps - 1]->timeWithinFrameUs;
}

void DeviceAnimator::doFrame()
{
    unsigned long preExecTimeUs;
    unsigned long postExecTimeUs;
    unsigned long execTimeDiffUs;
    unsigned long delayUs;

    // happens every frame because steps contents may change between frames; takes about 40 us
    initFrame();
    lastFrameCummulativeUndertime = 0;

    for (int i = 0; i < totalSteps; ++i) {
        // omit steps that should take 0 time
        if (stepsMerged[i]->runningTimeUs <= 0) {
            continue;
        }

        preExecTimeUs = micros();
        stepsMerged[i]->callback(stepsMerged[i]->devicePtr, stepsMerged[i]->sequenceNumber);
        postExecTimeUs = micros();
        execTimeDiffUs = postExecTimeUs >= preExecTimeUs ? postExecTimeUs - preExecTimeUs + EXEC_TIME_OVERHEAD_US : 0;

        // detect timeToNextMergedStepUs < execTimeDiffUs (means too small intervals specified)
        bool undertimeDetected = false;

        // for debug purposes report undertime
        if (stepsMerged[i]->timeToNextMergedStepUs < execTimeDiffUs) {
            undertimeDetected = true;
            lastFrameCummulativeUndertime += execTimeDiffUs - stepsMerged[i]->timeToNextMergedStepUs;
//            Serial.println(lastFrameCummulativeUndertime, DEC);
            if (hasFailureListener) {
                this->failureListener->undertimeFailureWarning(
                    stepsMerged[i]->timeToNextMergedStepUs,
                    execTimeDiffUs
                );
            }
        }

        // wait the appropriate amount of time for the next step (taking into account how much time it took to execute the step)
        delayUs = undertimeDetected ? 0 : stepsMerged[i]->timeToNextMergedStepUs - execTimeDiffUs;
        if (delayUs > 16000) {
            delay(delayUs / 1000);
        } else {
            delayMicroseconds(delayUs);
        }
    }
}
