#include "IV18Animator.h"

IV18Animator::IV18Animator(IV18Display* display)
{
    this->display = display;

    // TODO: these cause undertime - due to collision with lampDigitSteps
    //       think about rectangular heartbeat with a non-colliding offsets instead
    auto heartbeatSteps = new DeviceAnimatorStep[2]{
        // runningTimeUs values will be overriden on every frame by animateStatusLED
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
                           IV18I2CCommandExecutor::MAX_EXECUTION_TIME_US, 10)
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampDigitSteps, IV18Display::DIGIT_STEPS_COUNT * 2 + 2)
    };
    statusLedThread = &(threads[0]);
    lampDigitThread = &(threads[1]);

    animator = new DeviceAnimator(threads, 2);
}

IV18Display* IV18Animator::getDisplay()
{
    return this->display;
}

void IV18Animator::setFailureListener(AnimatorFailureListenerInterface *failureListener)
{
    animator->setFailureListener(failureListener);
}

void IV18Animator::animateLampDigitBrightnesses()
{
    short j;
    unsigned long frameLength = LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US;
    for (short i = 0; i < IV18Display::DIGIT_STEPS_COUNT; ++i) {
        j = 2*i + 1; //step number
        switch (lampDigitActions[i]) {
            case LAMP_MODE_DIGIT_STATIC:
                break;
            case LAMP_MODE_DIGIT_IN:
                lampDigitThread->steps[j].runningTimeUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    lampDigitMinOffDutyUs[i],
                    lampDigitMaxOnDutyUs[i],
                    frameLength,
                    lampDigitCurrentFrameInCycle[i],
                    lampDigitFramesPerCycle[i],
                    true,
                    true,
                    LAMP_DIGIT_CUTOUT_DUTY_US
                );
                lampDigitThread->steps[j - 1].runningTimeUs = frameLength - lampDigitThread->steps[j].runningTimeUs;

                // digit frame counting
                ++lampDigitCurrentFrameInCycle[i];
                if (lampDigitCurrentFrameInCycle[i] >= lampDigitFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampDigitCurrentFrameInCycle[i] = 0;
                    lampDigitActions[i] = LAMP_MODE_DIGIT_STATIC;
                }
                break;
            case LAMP_MODE_DIGIT_OUT:
                lampDigitThread->steps[j].runningTimeUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                    lampDigitMinOffDutyUs[i],
                    lampDigitMaxOnDutyUs[i],
                    frameLength,
                    lampDigitCurrentFrameInCycle[i],
                    lampDigitFramesPerCycle[i],
                    true,
                    false,
                    LAMP_DIGIT_CUTOUT_DUTY_US
                );
                lampDigitThread->steps[j - 1].runningTimeUs = frameLength - lampDigitThread->steps[j].runningTimeUs;

                ++lampDigitCurrentFrameInCycle[i];
                if (lampDigitCurrentFrameInCycle[i] >= lampDigitFramesPerCycle[i]) {
                    // switch to static on last frame
                    lampDigitCurrentFrameInCycle[i] = 0;
                    lampDigitActions[i] = LAMP_MODE_DIGIT_STATIC;
                }
                break;
        }
    }
}

void IV18Animator::animateStatusLED()
{
    // TODO: maybe make a runningTimeUs setter, only getters for the rest and make them private?
    switch (ledAction) {
        case LED_MODE_HEARTBEAT:
        case LED_MODE_WARNING:
            statusLedThread->steps[0].runningTimeUs = SinusoidalDutyCycleGenerator::animateSinusoidalDutyCycle(
                LED_MIN_DUTY_US[ledAction],
                LED_MAX_DUTY_US[ledAction],
                LED_FRAME_LENGTH_US,
                ledCurrentFrameInCycle,
                LED_FRAMES_PER_CYCLE[ledAction]
            );
            break;
        case LED_MODE_SQUARE_HEARTBEAT:
            if (ledCurrentFrameInCycle < LED_FRAMES_PER_CYCLE_MAX / 2) {
                statusLedThread->steps[0].runningTimeUs = LED_MAX_DUTY_SQUARE_US;
            } else {
                statusLedThread->steps[0].runningTimeUs = LED_MIN_DUTY_SQUARE_US;
            }
            break;
        case LED_MODE_DIM:
            // can't set it to 0 now, because of potential undertime (?orly?)
            statusLedThread->steps[0].runningTimeUs = LED_DUTY_DIM_US; // TODO: use a constant here?
            break;
    }
    statusLedThread->steps[1].runningTimeUs = LED_FRAME_LENGTH_US - statusLedThread->steps[0].runningTimeUs;

    ++ledCurrentFrameInCycle;
    // for now, we're doing uniform cycles per all threads (applies only to heartbeat atm)
    if (ledCurrentFrameInCycle >= LED_FRAMES_PER_CYCLE_MAX) {
        ledCurrentFrameInCycle = 0;
    }

    if ((ledAction == LED_MODE_WARNING) && (ledCurrentFrameInCycle % LED_FRAMES_PER_CYCLE[LED_MODE_WARNING] == 0)) {
        decreaseWarningBleeps();
    }
}

void IV18Animator::setLEDModeWarning(short bleepsCount)
{
    ledAction = LED_MODE_WARNING;
    warningBleepsLeft = bleepsCount;
}

void IV18Animator::decreaseWarningBleeps()
{
    if (warningBleepsLeft > 0) {
        if(--warningBleepsLeft == 0) {
            ledAction = LED_MODE_DEFAULT;
        }
    }
}

void IV18Animator::setCurrentLampDigitDutyValue(short lampDigitNumber, unsigned short dutyValue)
{
    // TODO: calculate it in constructor?
    unsigned long frameLength = LAMP_DIGIT_MAX_DUTY_US + LAMP_DIGIT_PREPARE_MIN_DUTY_US;
    this->lampDigitThread->steps[2 * lampDigitNumber + 1].runningTimeUs = dutyValue;
    this->lampDigitThread->steps[2 * lampDigitNumber].runningTimeUs = frameLength - dutyValue;
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

void IV18Animator::doCurrentSequencing()
{
    if (sequencingEnabled) {
        sequencingCallback(this);
    }
}

void IV18Animator::setSequencingCallback(void (*sequencingCallbackToSet)(IV18Animator *))
{
    sequencingCallback = sequencingCallbackToSet;
    sequencingEnabled = true;
}

void IV18Animator::disableSequencing()
{
    sequencingEnabled = false;
}

unsigned short IV18Animator::getLampDigitPreviousOnDutyUs(byte lampDigitNumber)
{
    // TODO: throw exception on unsupported index?
    return this->lampDigitThread->steps[2 * lampDigitNumber + 1].runningTimeUs;
}

void IV18Animator::setLampDigitFramesPerAction(short lampDigitNumber, unsigned short lampDigitFramesPerAction)
{
    lampDigitFramesPerCycle[lampDigitNumber] = lampDigitFramesPerAction;
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateStatusLED();
    // multiframe digit brightness animation (static, fade-in, fade-out etc.)
    animateLampDigitBrightnesses();

    animator->doFrame();
}