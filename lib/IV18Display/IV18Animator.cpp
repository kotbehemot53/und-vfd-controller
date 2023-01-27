#include "IV18Animator.h"

#include "../DutyCycleGenerator/SinusoidalDutyCycleGenerator.h"

IV18Animator::IV18Animator(IV18Display &display)
{
    // TODO: preprocessor could do it
    // determine longest multiframe animation cycle
    framesPerLongestCycle = 0;
    for (unsigned short i : FRAMES_PER_CYCLE) {
        if (i > framesPerLongestCycle) {
            framesPerLongestCycle = i;
        }
    }

    auto heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateStatusLED
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 4500),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 4500)
    };

    auto lampGridSteps = new DeviceAnimatorStep[IV18Display::GRID_STEPS_COUNT*2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 8)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };
    statusLedThread = &(threads[0]);
    lampGridThread = &(threads[1]);

    animator.setThreads(threads, 2);
}

void IV18Animator::animateStatusLED()
{
    // TODO: maybe make a waitUs setter, only getters for the rest and make them private?
    statusLedThread->steps[0].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
        MIN_DUTY_US[ledAction],
        MAX_DUTY_US[ledAction],
        FRAME_LENGTH_US,
        currentFrame,
        FRAMES_PER_CYCLE[ledAction]
    );
    statusLedThread->steps[1].waitUs = FRAME_LENGTH_US - statusLedThread->steps[0].waitUs;
}

void IV18Animator::doWarning(short bleepsCount)
{
    ledAction = LED_WARNING;
    warningBleepsLeft = bleepsCount;
}

void IV18Animator::decreaseWarningBleeps()
{
    if (warningBleepsLeft > 0) {
        if(--warningBleepsLeft == 0) {
            ledAction = LED_HEARTBEAT;
        }
    }
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateStatusLED();

    animator.doFrame();

    ++currentFrame;

    // for now, we're doing uniform cycles per all threads (applies only to heartbeat atm)
    if (currentFrame >= framesPerLongestCycle) {
        currentFrame = 0;
    }

    if (currentFrame % FRAMES_PER_CYCLE[LED_WARNING] == 0) {
        decreaseWarningBleeps();
    }
}
