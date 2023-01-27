#include "IV18AnimatorSerialFailureListener.h"

#include "Arduino.h"

IV18AnimatorSerialFailureListener AnimatorFailureListener;

void IV18AnimatorSerialFailureListener::failureWarning(short warningCode)
{
    const char* message;

    switch (warningCode) {
        case AnimatorFailureListenerInterface::WARNING_UNDERTIME:
            message = "Animation subframe undertime!";
            break;
        default:
            message = "Unknown animation failure!";
    }

    Serial.println(message);
}
