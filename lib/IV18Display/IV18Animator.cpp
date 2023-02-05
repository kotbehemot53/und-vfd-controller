#include "IV18Animator.h"

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

IV18Animator::IV18Animator(IV18Display* display)
{
    this->display = display;

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
//    for (unsigned short i : lampDigitFramesPerCycle) {
//        if (i > lampGridFramesPerLongestCycle) {
//            lampGridFramesPerLongestCycle = i;
//        }
//    }

    // TODO: these cause undertime - due to collision with lampDigitSteps
    //       think about rectangular heartbeat with a non-colliding offsets instead
    auto heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateStatusLED
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 5000),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 5000)
    };

    // prepare must be at least 150 us - otherwise undertime happens constantly
    auto lampDigitSteps = new DeviceAnimatorStep[IV18Display::DIGIT_STEPS_COUNT * 2 + 2]{
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 0),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 0),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 1),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 1),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 2),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 2),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 3),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 3),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 4),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 4),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 5),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 5),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 6),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 6),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US, 7),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 7),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextDigitStep),
                           LAMP_DIGIT_PREPARE_MIN_DUTY_US + 1, 8),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doDigitStep),
                           LAMP_DIGIT_MAX_DUTY_US, 8),
           //TODO: perhaps smaller, minimal cooldown should be set here (below 100)? especially now, that we have the command processing that takes time
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::grid9OffAndCooldown),
                           IV18Display::GRID9_COOLDOWN_US, 9),
        DeviceAnimatorStep(this, reinterpret_cast<void (*)(void*,int)>(&IV18I2CCommandExecutor::executeBufferedCommands),
        IV18I2CCommandExecutor::MAX_EXECUTION_TIME, 10)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampDigitSteps, IV18Display::DIGIT_STEPS_COUNT * 2 + 2)
    };
    statusLedThread = &(threads[0]);
    lampDigitThread = &(threads[1]);

    animator.setThreads(threads, 2);

    // TODO: do we need this silliness?
//    randomSeed(analogRead(A2));
//    shuffleArray(randomLampGridOrder, IV18Display::DIGIT_STEPS_COUNT);
}

IV18Display* IV18Animator::getDisplay()
{
    return this->display;
}

void IV18Animator::setFailureListener(AnimatorFailureListenerInterface *failureListener)
{
    animator.setFailureListener(failureListener);
}

// TODO: do we need this silliness?
/**
 * This is just a silly animation of the grids
 */
//void IV18Animator::animateLampDigitBrightnesses()
//{
//    // TODO: add more animations/effects based on a state property
//
//    unsigned short j;
//
//    // 4 just because
//    for (short i = 0; i < 4; ++i) {
//        j = randomLampGridOrder[i];
//        lampDigitThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
//            10,
//            LAMP_DIGIT_MAX_DUTY_US,
//            LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US,
//            ledCurrentFrame + 30 * i, // just because TODO: add some randomness?
//            LAMP_GRID_FRAMES_PER_CYCLE // just because
//        );
//        lampDigitThread->steps[j-1].waitUs = LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US - lampDigitThread->steps[i].waitUs;
//    }
//}

void IV18Animator::animateLampDigitBrightnesses()
{
    short j;
    unsigned long frameLength = LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US;
    for (short i = 0; i < IV18Display::DIGIT_STEPS_COUNT; ++i) {
        j = 2*i + 1; //step number
        switch (lampDigitActions[i]) {
            case LAMP_DIGIT_STATIC:
                break;
            case LAMP_DIGIT_IN:
                lampDigitThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    lampDigitMinOffDutyUs[i],
                    lampDigitMaxOnDutyUs[i],
                    frameLength,
                    lampDigitCurrentFrameInCycle[i],
                    lampDigitFramesPerCycle[i],
                    true,
                    true,
                    LAMP_DIGIT_CUTOUT_DUTY_US
                );
                lampDigitThread->steps[j - 1].waitUs = frameLength - lampDigitThread->steps[j].waitUs;

                // digit frame counting
                ++lampDigitCurrentFrameInCycle[i];
                if (lampDigitCurrentFrameInCycle[i] >= lampDigitFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampDigitCurrentFrameInCycle[i] = 0;
                    lampDigitActions[i] = LAMP_DIGIT_STATIC;
                }
                break;
            case LAMP_DIGIT_OUT:
                lampDigitThread->steps[j].waitUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    lampDigitMinOffDutyUs[i],
                    lampDigitMaxOnDutyUs[i],
                    frameLength,
                    lampDigitCurrentFrameInCycle[i],
                    lampDigitFramesPerCycle[i],
                    true,
                    false,
                    LAMP_DIGIT_CUTOUT_DUTY_US
                );
                lampDigitThread->steps[j - 1].waitUs = frameLength - lampDigitThread->steps[j].waitUs;

                ++lampDigitCurrentFrameInCycle[i];
                if (lampDigitCurrentFrameInCycle[i] >= lampDigitFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampDigitCurrentFrameInCycle[i] = 0;
                    lampDigitActions[i] = LAMP_DIGIT_STATIC;
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
            LED_FRAME_LENGTH_US,
            ledCurrentFrame,
            LED_FRAMES_PER_CYCLE[ledAction]
        );
        statusLedThread->steps[1].waitUs = LED_FRAME_LENGTH_US - statusLedThread->steps[0].waitUs;
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

//void IV18Animator::setLampGridOnDutyValues(const unsigned short *values)
//{
//    for (short i = 0; i < IV18Display::DIGIT_STEPS_COUNT; ++i) {
//        this->lampDigitMaxOnDutyUs[i] = values[i];
//    }
//}

void IV18Animator::setCurrentLampDigitDutyValue(short lampGridNumber, unsigned short dutyValue)
{
    // TODO: calculate it in constructor?
    unsigned long frameLength = LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US;
    this->lampDigitThread->steps[2 * lampGridNumber + 1].waitUs = dutyValue;
    this->lampDigitThread->steps[2 * lampGridNumber].waitUs = frameLength - dutyValue;
}

void IV18Animator::setLampDigitAction(
    short lampDigitNumber,
    short action,
    unsigned short maxDutyValue,
    unsigned short minDutyValue
)
{
    this->lampDigitMaxOnDutyUs[lampDigitNumber] = maxDutyValue;
    this->lampDigitMinOffDutyUs[lampDigitNumber] = minDutyValue;

    this->lampDigitActions[lampDigitNumber] = action;
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateStatusLED();
    animateLampDigitBrightnesses();

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
