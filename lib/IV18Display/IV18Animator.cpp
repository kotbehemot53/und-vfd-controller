#include "IV18Animator.h"

#include "../DutyCycleGenerator/SinusoidalDutyCycleGenerator.h"

// TODO: do we need this silliness?
//void IV18Animator::shuffleArray(unsigned short * array, int size)
//{
//    int last = 0;
//    unsigned short temp = array[last];
//    for (int i = 0; i < size; ++i)
//    {
//        int index = random(size);
//        array[last] = array[index];
//        last = index;
//    }
//    array[last] = temp;
//}

IV18Animator::IV18Animator(IV18Display &display)
{
    // TODO: preprocessor could do it
    // determine longest multiframe animation cycle
    ledFramesPerLongestCycle = 0;
    for (unsigned short i : LED_FRAMES_PER_CYCLE) {
        if (i > ledFramesPerLongestCycle) {
            ledFramesPerLongestCycle = i;
        }
    }
    // TODO: not needed, right?
//    lampGridFramesPerLongestCycle = 0;
//    for (unsigned short i : lampGridFramesPerCycle) {
//        if (i > lampGridFramesPerLongestCycle) {
//            lampGridFramesPerLongestCycle = i;
//        }
//    }

    // TODO: these cause undertime - due to collision with lampGridSteps?
    auto heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateStatusLED
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 5000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 5000)
    };

    // prepare must be at least 150 us - otherwise undertime happens constantly
    auto lampGridSteps = new DeviceAnimatorStep[IV18Display::GRID_STEPS_COUNT*2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep),
                           LAMP_GRID_PREPARE_MIN_DUTY_US + 1, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep),
                           LAMP_GRID_MAX_DUTY_US, 8)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };
    statusLedThread = &(threads[0]);
    lampGridThread = &(threads[1]);

    animator.setThreads(threads, 2);

    // TODO: do we need this silliness?
//    randomSeed(analogRead(A2));
//    shuffleArray(randomLampGridOrder, IV18Display::GRID_STEPS_COUNT);
}

void IV18Animator::setFailureListener(AnimatorFailureListenerInterface *failureListener)
{
    animator.setFailureListener(failureListener);
}

// TODO: do we need this silliness?
/**
 * This is just a silly animation of the grids
 */
//void IV18Animator::animateLampGridBrightnesses()
//{
//    // TODO: add more animations/effects based on a state property
//
//    unsigned short j;
//
//    // 4 just because
//    for (short i = 0; i < 4; ++i) {
//        j = randomLampGridOrder[i];
//        lampGridThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
//            10,
//            LAMP_GRID_MAX_DUTY_US,
//            LAMP_GRID_MAX_DUTY_US + LAMP_GRID_PREPARE_MIN_DUTY_US,
//            ledCurrentFrame + 30 * i, // just because TODO: add some randomness?
//            LAMP_GRID_FRAMES_PER_CYCLE // just because
//        );
//        lampGridThread->steps[j-1].waitUs = LAMP_GRID_MAX_DUTY_US + LAMP_GRID_PREPARE_MIN_DUTY_US - lampGridThread->steps[i].waitUs;
//    }
//}

void IV18Animator::animateLampGridBrightnesses()
{
    short j;
    unsigned long frameLength = LAMP_GRID_MAX_DUTY_US + LAMP_GRID_PREPARE_MIN_DUTY_US;
    for (short i = 0; i < IV18Display::GRID_STEPS_COUNT; ++i) {
        j = 2*i + 1; //step number
        switch (lampGridActions[i]) {
            case LAMP_GRID_STATIC:
//                lampGridThread->steps[j].waitUs =
                break;
            case LAMP_GRID_IN:
                lampGridThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    0,
                    lampGridMaxOnDutyUs[i],
                    frameLength,
                    lampGridCurrentFrameInCycle[i],
                    lampGridFramesPerCycle[i],
                    true,
                    true
                );
                lampGridThread->steps[j-1].waitUs = frameLength - lampGridThread->steps[i].waitUs;

                // digit frame counting
                ++lampGridCurrentFrameInCycle[i];
                if (lampGridCurrentFrameInCycle[i] >= lampGridFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampGridCurrentFrameInCycle[i] = 0;
                    lampGridActions[i] = LAMP_GRID_STATIC;
                }
                break;
            case LAMP_GRID_OUT:
                lampGridThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    0,
                    lampGridMaxOnDutyUs[i],
                    frameLength,
                    lampGridCurrentFrameInCycle[i],
                    lampGridFramesPerCycle[i],
                    true,
                    false
                );
                lampGridThread->steps[j-1].waitUs = frameLength - lampGridThread->steps[i].waitUs;

                ++lampGridCurrentFrameInCycle[i];
                if (lampGridCurrentFrameInCycle[i] >= lampGridFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampGridCurrentFrameInCycle[i] = 0;
                    lampGridActions[i] = LAMP_GRID_STATIC;
                }
                break;
        }
    }
}

void IV18Animator::animateStatusLED()
{
    // TODO: maybe make a waitUs setter, only getters for the rest and make them private?
    if (ledAction < LED_SINUS_MODES_COUNT) {
        statusLedThread->steps[0].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
            LED_MIN_DUTY_US[ledAction],
            LED_MAX_DUTY_US[ledAction],
            FRAME_LENGTH_US,
            ledCurrentFrame,
            LED_FRAMES_PER_CYCLE[ledAction]
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

void IV18Animator::setLampGridOnDutyValues(const unsigned short *values)
{
    for (short i = 0; i < IV18Display::GRID_STEPS_COUNT; ++i) {
        this->lampGridMaxOnDutyUs[i] = values[i];
    }
}

void IV18Animator::setCurrentLampGridDutyValue(short lampGridNumber, unsigned short dutyValue)
{
    // TODO: calculate it in constructor?
    unsigned long frameLength = LAMP_GRID_MAX_DUTY_US + LAMP_GRID_PREPARE_MIN_DUTY_US;
    this->lampGridThread->steps[2 * lampGridNumber + 1].waitUs = dutyValue;
    this->lampGridThread->steps[2 * lampGridNumber].waitUs = frameLength - dutyValue;
}

void IV18Animator::setLampGridAction(short lampGridNumber, short action)
{
    this->lampGridActions[lampGridNumber] = action;
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateStatusLED();
    animateLampGridBrightnesses();

    animator.doFrame();

    ++ledCurrentFrame;
    // for now, we're doing uniform cycles per all threads (applies only to heartbeat atm)
    if (ledCurrentFrame >= ledFramesPerLongestCycle) {
        ledCurrentFrame = 0;
    }



    // TODO: separate cycle & bleeps for LED_KILL?
    if (ledCurrentFrame % LED_FRAMES_PER_CYCLE[LED_WARNING] == 0) {
        decreaseWarningBleeps();
    }
}
