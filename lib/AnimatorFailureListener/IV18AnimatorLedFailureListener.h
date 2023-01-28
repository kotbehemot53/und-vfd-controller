#ifndef PIUCNTVFD1_IV18ANIMATORLEDFAILURELISTENER_H
#define PIUCNTVFD1_IV18ANIMATORLEDFAILURELISTENER_H

#include "AnimatorFailureListenerInterface.h"
#include "../IV18Display/IV18Animator.h"

// TODO: probably doesn't make sense and should be removed due to it causing undertimes itself
//       unless it somehow bypasses the animator

class IV18AnimatorLedFailureListener : public AnimatorFailureListenerInterface
{
private:
    IV18Animator* iv18Animator;

public:
    void failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime) override;

    explicit IV18AnimatorLedFailureListener(IV18Animator* iv18Animator) : iv18Animator(iv18Animator) {
        iv18Animator->setFailureListener(this);
    };
};

#endif //PIUCNTVFD1_IV18ANIMATORLEDFAILURELISTENER_H
