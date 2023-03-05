#ifndef PIUCNTVFD1_IV18INTROSEQUENCER_H
#define PIUCNTVFD1_IV18INTROSEQUENCER_H

#include "IV18Animator.h"

class IV18IntroSequencer
{
private:
    static unsigned long currentFrame;
    static int animSteps[4];

public:
    static void doIntroSequencing(IV18Animator* animator);
};

#endif //PIUCNTVFD1_IV18INTROSEQUENCER_H
