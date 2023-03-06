#ifndef PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
#define PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H


class AnimatorFailureListenerInterface
{
public:
    static const short WARNING_UNDERTIME = 0;

    virtual void failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime) = 0;
};


#endif //PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
