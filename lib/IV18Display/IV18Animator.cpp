#include "IV18Animator.h"

#include "../DutyCycleGenerator/SinusoidalDutyCycleGenerator.h"

void IV18Animator::shuffleArray(unsigned short * array, int size)
{
    int last = 0;
    int temp = array[last];
    for (int i=0; i<size; i++)
    {
        int index = random(size);
        array[last] = array[index];
        last = index;
    }
    array[last] = temp;
}

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
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           PREPARE_MIN_DUTY_US + 1, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           GRID_MAX_DUTY_US, 8)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };
    statusLedThread = &(threads[0]);
    lampGridThread = &(threads[1]);

    animator.setThreads(threads, 2);

    randomSeed(analogRead(A2));
    shuffleArray(randomLampGridOrder, IV18Display::GRID_STEPS_COUNT);
}

void IV18Animator::setFailureListener(AnimatorFailureListenerInterface *failureListener)
{
    animator.setFailureListener(failureListener);
}

/**
 * This is just a silly animation of the grids
 */
void IV18Animator::animateGridsBrightness()
{
    // TODO: add more animations/effects based on a state property

    unsigned short j;

    // 4 just because
    for (short i = 0; i < 4; ++i) {
        j = randomLampGridOrder[i];
        lampGridThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
            10,
            GRID_MAX_DUTY_US,
            GRID_MAX_DUTY_US + PREPARE_MIN_DUTY_US,
            currentFrame + 30 * i, // just because TODO: add some randomness?
            LAMP_GRID_FRAMES_PER_CYCLE // just because
        );
        lampGridThread->steps[j-1].waitUs = GRID_MAX_DUTY_US + PREPARE_MIN_DUTY_US - lampGridThread->steps[i].waitUs;
    }
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

//void IV18Animator::animate

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

    // TODO: enable it by command?
    animateGridsBrightness();

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
