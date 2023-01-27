#include "IV18AnimatorSerialFailureListener.h"

#include "Arduino.h"

// TODO: the times don't make much sense for other potential codes; also will there EVER be other codes?
void IV18AnimatorSerialFailureListener::failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime)
{
    if (warningCode == AnimatorFailureListenerInterface::WARNING_UNDERTIME) {
        // TODO: sprintf maybe?
        Serial.print("Animation subframe undertime! Expected time: ");
        Serial.print(expectedTime);
        Serial.print(", actual time: ");
        Serial.println(actualTime);
        return;
    }

    Serial.println("Unknown animation failure!");
}
