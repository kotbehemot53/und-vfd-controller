#ifndef PIUCNTVFD1_DEVICEANIMATORSTEP_H
#define PIUCNTVFD1_DEVICEANIMATORSTEP_H

class DeviceAnimatorStep
{
public:
    void* devicePtr;
    void (*callback)(void* devicePtr,int stepInThread); //void (*callback)(DeviceAnimatorStep &step); - self mod not there - do it here on running of step!
    unsigned long waitUs;

    unsigned long timeWithinFrameUs = 0;
    unsigned long timeToNextMergedStepUs = 0;
    int stepInThread = 0;

    DeviceAnimatorStep(void* callbackObjPtrToSet, void (*callbackToSet)(void*,int), unsigned long waitUsToSet) : devicePtr(callbackObjPtrToSet), callback(callbackToSet), waitUs(waitUsToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORSTEP_H
