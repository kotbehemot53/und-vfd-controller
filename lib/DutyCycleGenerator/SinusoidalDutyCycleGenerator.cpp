#include "SinusoidalDutyCycleGenerator.h"

#include <Arduino.h>

unsigned long SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
    unsigned long minDuty,
    unsigned long maxDuty,
    unsigned long frameLength,
    unsigned short currentFrame,
    unsigned short framesPerCycle
)
{
    // formula for a nice sinusoidal bajinga
    //((maxDuty - minDuty)/2)* (1 - cos(x/(0.0506599 * framesPerCycle * π))) + minDuty

    unsigned long onTime;
    double multiplier = 1 - cos(currentFrame / (0.0506599 * framesPerCycle * PI));
    onTime = multiplier <= 0 ? minDuty : (unsigned long)(((maxDuty - minDuty) / 2) * multiplier) + minDuty;

    if (onTime > frameLength) {
        onTime = frameLength;
    }

    return onTime;
}