#ifndef PIUCNTVFD1_DEVICEANIMATORSTEP_H
#define PIUCNTVFD1_DEVICEANIMATORSTEP_H

/**
 * A representation of a single animation step in a single animation thread.
 */
class DeviceAnimatorStep
{
private:
    void (*callback)(void* callerPtr, int sequenceNumber); // a callback called for the given step (it should be passed the caller pointer and a number in steps sequence)
    void* callerPtr; // caller pointer to be passed to the callback
    int sequenceNumber = 0; // a number in the animation sequence to be passed to the callback

public:
    unsigned long runningTimeUs; // total intended step running time

    unsigned long timeWithinFrameUs = 0; // calculated time within the frame when the step should start
    unsigned long timeToNextMergedStepUs = 0; // calculated time before next step is called

    DeviceAnimatorStep(void* callbackObjPtrToSet, void (*callbackToSet)(void*,int), unsigned long waitUsToSet, int sequenceNumberToSet = 0) : callback(callbackToSet), callerPtr(callbackObjPtrToSet), sequenceNumber(sequenceNumberToSet), runningTimeUs(waitUsToSet) {};

    /**
     * Run the animation step
     */
    inline void runCallback()
    {
        this->callback(this->callerPtr, this->sequenceNumber);
    }
};

#endif //PIUCNTVFD1_DEVICEANIMATORSTEP_H
