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

    // TODO: these cause undertime - due to collision with lampGridSteps?
    auto heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateStatusLED
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 5000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 5000)
    };

    // prepare must be at least 150 us - otherwise undertime happens constantly
    auto lampGridSteps = new DeviceAnimatorStep[IV18Display::GRID_STEPS_COUNT*2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 150, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 151, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 961, 8)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };
    statusLedThread = &(threads[0]);
    lampGridThread = &(threads[1]);

    animator.setThreads(threads, 2);
}

void IV18Animator::setFailureListener(AnimatorFailureListenerInterface *failureListener)
{
    animator.setFailureListener(failureListener);
}

void IV18Animator::animateStatusLED()
{
    // TODO: maybe make a waitUs setter, only getters for the rest and make them private?
    if (ledAction < LED_SINUS_MODES_COUNT) {
        statusLedThread->steps[0].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
            MIN_DUTY_US[ledAction],
            MAX_DUTY_US[ledAction],
            FRAME_LENGTH_US,
            currentFrame,
            FRAMES_PER_CYCLE[ledAction]
        );
        statusLedThread->steps[1].waitUs = FRAME_LENGTH_US - statusLedThread->steps[0].waitUs;
    } else {
        if (ledAction == LED_KILL) {
            // TODO: implement actual total kills somehow? omittable threads?
            // can't set it to 0 now, because of potential undertime
            statusLedThread->steps[0].waitUs = 200;
            statusLedThread->steps[1].waitUs = 8800;
        }
    }
}

void IV18Animator::doWarning(short bleepsCount)
{
    ledAction = LED_WARNING;
    warningBleepsLeft = bleepsCount;
}

void IV18Animator::doKillLED()
{
    ledAction = LED_KILL;
    warningBleepsLeft = 1;
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

    // TODO: separate cycle & bleeps for LED_KILL?
    if (currentFrame % FRAMES_PER_CYCLE[LED_WARNING] == 0) {
        decreaseWarningBleeps();
    }
}
