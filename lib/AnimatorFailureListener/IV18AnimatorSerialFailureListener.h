#ifndef PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
#define PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H

#include "AnimatorFailureListenerInterface.h"

class IV18AnimatorSerialFailureListener : public AnimatorFailureListenerInterface
{
public:
    void failureWarning(short warningCode) override;

};

extern IV18AnimatorSerialFailureListener AnimatorFailureListener;

#endif //PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
