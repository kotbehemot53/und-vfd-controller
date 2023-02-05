#ifndef PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
#define PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H

#include "IV18Animator.h"

class IV18Animator;

class IV18I2CCommandExecutor
{
public:
    // TODO: check if it's actually sufficient (no undertimes in typical scenarios, no led of course)
    static const short MAX_EXECUTION_TIME = 500;

    static void executeBufferedCommands(IV18Animator* animator, int sequenceNumber);
};

#endif //PIUCNTVFD1_IV18I2CCOMMANDEXECUTOR_H
