#include "IV18AnimatorLedFailureListener.h"

void IV18AnimatorLedFailureListener::failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime)
{
    iv18Animator->doKillLED();
}
