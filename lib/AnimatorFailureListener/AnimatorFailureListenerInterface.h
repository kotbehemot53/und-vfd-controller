#ifndef PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
#define PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H

/**
 * A generic interface for failure listeners that can be notified of DeviceAnimator undertimes.
 */
class AnimatorFailureListenerInterface
{
public:
    virtual void undertimeFailureWarning(unsigned long expectedTime, unsigned long actualTime) = 0;
};


#endif //PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
