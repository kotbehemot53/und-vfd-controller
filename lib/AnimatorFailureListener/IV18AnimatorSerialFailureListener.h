#ifndef PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
#define PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H

#include "AnimatorFailureListenerInterface.h"
#include "../IV18Display/IV18Animator.h"

/**
 * This one can be used for debug
 */
class IV18AnimatorSerialFailureListener : public AnimatorFailureListenerInterface
{
private:
    IV18Animator* iv18Animator;

public:
    void failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime) override;

    // TODO: move this constructor to an IV18-generic base class? (for both listeners)
    explicit IV18AnimatorSerialFailureListener(IV18Animator* iv18Animator) : iv18Animator(iv18Animator) {
        iv18Animator->setFailureListener(this);
    };
};

#endif //PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
