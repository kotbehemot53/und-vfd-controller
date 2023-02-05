#ifndef PIUCNTVFD1_DEVICEANIMATORSTEP_H
#define PIUCNTVFD1_DEVICEANIMATORSTEP_H

class DeviceAnimatorStep
{
public:
    void* devicePtr;
    void (*callback)(void* callerPtr, int sequenceNumber);
    unsigned long waitUs;
    int sequenceNumber = 0;

    unsigned long timeWithinFrameUs = 0;
    unsigned long timeToNextMergedStepUs = 0;

    DeviceAnimatorStep(void* callbackObjPtrToSet, void (*callbackToSet)(void*,int), unsigned long waitUsToSet, int sequenceNumberToSet = 0) : devicePtr(callbackObjPtrToSet), callback(callbackToSet), waitUs(waitUsToSet), sequenceNumber(sequenceNumberToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORSTEP_H
