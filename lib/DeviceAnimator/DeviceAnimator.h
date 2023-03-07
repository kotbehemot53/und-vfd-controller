#ifndef PIUCNTVFD1_DEVICEANIMATOR_H
#define PIUCNTVFD1_DEVICEANIMATOR_H

#include "DeviceAnimatorThread.h"
#include "../AnimatorFailureListener/AnimatorFailureListenerInterface.h"

/**
 * A generic device animation planner/executor. It's initialised with a number of parallel animation threads
 * consisting of animation steps.
 * It merges all threads into a single thread and runs the steps in the appropriate sequence, maintaning intended
 * step durations.
 *
 * Caution: "undertimes" may occur, when 2 steps have too little time planned between them to run them.
 */
class DeviceAnimator
{
private:
    // overhead of all calculations after calculating the exec time but before actually doing the final wait/delay
    // determined experimentally
    static const int EXEC_TIME_OVERHEAD_US = 8;

    DeviceAnimatorThread* threads;
    int numberOfThreads = 0;
    DeviceAnimatorStep** stepsMerged;
    int totalSteps = 0;
    unsigned long frameEndTimeUs = 0;
    unsigned long lastFrameCummulativeUndertime = 0;

    AnimatorFailureListenerInterface* failureListener;
    bool hasFailureListener = false;

    static int compareMergedSteps(const void *cmp1, const void *cmp2);
    void initFrame();

public:
    DeviceAnimator(DeviceAnimatorThread threadsToSet[], int numberOfThreadsToSet);

    /**
     * Set a listener to react to timing failures (undertimes in the merged animation sequence).
     *
     * @param failureListener
     */
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    /**
     * Animate a frame (perform all steps from all threads in the correct order, for the correct time).
     */
    void doFrame();
};

#endif //PIUCNTVFD1_DEVICEANIMATOR_H
