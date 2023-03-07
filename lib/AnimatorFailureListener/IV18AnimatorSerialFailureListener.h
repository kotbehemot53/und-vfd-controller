#ifndef PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
#define PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H

#include "AnimatorFailureListenerInterface.h"
#include "../IV18Display/IV18Animator.h"

/**
 * A failure listener, which, when animation undertime occurs, sends a message to the serial interface.
 * Meant to be used with the IV18Animator.
 */
class IV18AnimatorSerialFailureListener : public AnimatorFailureListenerInterface
{
private:
    IV18Animator* iv18Animator;

public:
    void undertimeFailureWarning(unsigned long expectedTime, unsigned long actualTime) override;

    // TODO: move this constructor to an IV18-generic base class? (for both listeners)
    explicit IV18AnimatorSerialFailureListener(IV18Animator* iv18Animator) : iv18Animator(iv18Animator) {
        iv18Animator->setFailureListener(this);
    };
};

#endif //PIUCNTVFD1_IV18ANIMATORSERIALFAILURELISTENER_H
