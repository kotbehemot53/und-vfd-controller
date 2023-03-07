#ifndef PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H
#define PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H

/**
 * Utility class to generate a sinusoidal changing duty cycles (or half cycles) between given min and max values.
 */
class SinusoidalDutyCycleGenerator
{
public:
    /**
     * Generates a on-duty cycle value in microseconds for a given frame of a sinusoidal changing cycle.
     *
     * @param minDuty minimal on-duty value for the sinusoid
     * @param maxDuty maximum on-duty value for the sinusoid
     * @param frameLength full single subcycle/frame length in microseconds (off + on time)
     * @param currentFrame number of the current frame within the sinusoidal cycle
     * @param framesPerCycle number of frames per full sinusoidal cycle
     * @param isHalfCycle whether we want to only do a half-cycle (fade-in or fade-out)
     * @param isUp whether we're starting up (fade-in) or down (fade-out)
     * @param cutoutOnAndBelow on-duty value in microseconds for which we cut out the output completely (on-duty for this and lower values is converted to 0)
     *
     * @return on-duty time in microseconds for the current subcycle/frame
     */
    static unsigned long animateSinusoidalDutyCycle(
        unsigned long minDuty,
        unsigned long maxDuty,
        unsigned long frameLength,
        unsigned short currentFrame,
        unsigned short framesPerCycle,
        bool isHalfCycle = false,
        bool isUp = true,
        unsigned long cutoutOnAndBelow = 100
    );
};

#endif //PIUCNTVFD1_SINUSOIDALDUTYCYCLEGENERATOR_H
