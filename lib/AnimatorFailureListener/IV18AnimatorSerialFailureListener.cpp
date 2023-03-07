#include "IV18AnimatorSerialFailureListener.h"

#include "Arduino.h"

void IV18AnimatorSerialFailureListener::undertimeFailureWarning(unsigned long expectedTime, unsigned long actualTime)
{
    // TODO: sprintf maybe?
    Serial.print("Animation subframe undertime! Expected time: ");
    Serial.print(expectedTime);
    Serial.print(", actual time: ");
    Serial.println(actualTime);
}
