#include "SinusoidalDutyCycleGenerator.h"

#include <Arduino.h>

unsigned long SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
    unsigned long minDuty,
    unsigned long maxDuty,
    unsigned long frameLength,
    unsigned short currentFrame,
    unsigned short framesPerCycle,
    bool isHalfCycle,
    bool isUp
)
{
    // formula for a nice sinusoidal bajinga
    //((maxDuty - minDuty)/2)* (1 - cos((x + horizontalOffset)/(0.0506599 * framesPerCycle * horizontalMultiplier * π))) + minDuty

    unsigned short horizontalOffset = 0;
    int horizontalMultiplier = 1;
    if (isHalfCycle) {
        horizontalMultiplier = 2;
    }
    if (!isUp) {
        horizontalOffset = -framesPerCycle;
    }

    unsigned long onTime;
    double multiplier = 1 - cos((currentFrame + horizontalOffset) / (0.0506599 * framesPerCycle * horizontalMultiplier * PI));
    onTime = multiplier <= 0 ? minDuty : (unsigned long)(((maxDuty - minDuty) / 2) * multiplier) + minDuty;

    if (onTime > frameLength) {
        onTime = frameLength;
    }

    return onTime;
}