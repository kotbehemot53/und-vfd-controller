#ifndef PIUCNTVFD1_DEVICEANIMATORSTEP_H
#define PIUCNTVFD1_DEVICEANIMATORSTEP_H

class DeviceAnimatorStep
{
public:
    void* callbackObjPtr;
    void (*callback)(void*); //void (*callback)(DeviceAnimatorStep &step); - self mod not there - do it here on running of step!
    unsigned long waitUs;

    DeviceAnimatorStep(void* callbackObjPtrToSet, void (*callbackToSet)(void*), unsigned long waitUsToSet) : callbackObjPtr(callbackObjPtrToSet), callback(callbackToSet), waitUs(waitUsToSet) {};
};

#endif //PIUCNTVFD1_DEVICEANIMATORSTEP_H
