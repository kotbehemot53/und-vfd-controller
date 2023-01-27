#ifndef PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H
#define PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H

class SinusoidalDutyCycleGenerator
{
public:
    static unsigned long animateSinusoidalDutyCycle(
        unsigned long minDuty,
        unsigned long maxDuty,
        unsigned long frameLength,
        unsigned short currentFrame,
        unsigned short framesPerCycle
    );
};

#endif //PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H