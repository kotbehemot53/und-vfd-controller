#include "IV18AnimatorSerialFailureListener.h"

#include "Arduino.h"

// TODO: the times don't make much sense for other potential codes; also will there EVER be other codes?
void IV18AnimatorSerialFailureListener::failureWarning(short warningCode, unsigned long expectedTime, unsigned long actualTime)
{
    const char* message;
    char str[1000];

    switch (warningCode) {
        case AnimatorFailureListenerInterface::WARNING_UNDERTIME:
            message = "Animation subframe undertime! Expected time: %d, actual time: %d";
            sprintf(str, message, expectedTime, actualTime);
            break;
        default:
            message = "Unknown animation failure!";
            sprintf(str, message);
    }

    Serial.println(str);
}
