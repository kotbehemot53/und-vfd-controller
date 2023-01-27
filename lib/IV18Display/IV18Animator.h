#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18Display.h"

class IV18Animator
{
private:
    static const short LED_HEARTBEAT = 0;
    static const short LED_WARNING = 1;
    static const short LED_SINUS_MODES_COUNT = 2;

    static const short LED_KILL = 100;

    static const unsigned long FRAME_LENGTH_US = 9000; // us -> just below 1/100 s

    static constexpr unsigned short FRAMES_PER_CYCLE[LED_SINUS_MODES_COUNT] = {150, 30};
    static constexpr unsigned long MIN_DUTY_US[LED_SINUS_MODES_COUNT] = {1000, 0};
    static constexpr unsigned long MAX_DUTY_US[LED_SINUS_MODES_COUNT] = {5000, 9000};

    DeviceAnimator animator;
    unsigned short currentFrame = 0;

    // TODO: how to make it static and initialize it? preprocessor?
    unsigned short framesPerLongestCycle;

    DeviceAnimatorThread* threads;
    DeviceAnimatorThread* statusLedThread;
    DeviceAnimatorThread* lampGridThread;

    unsigned short ledAction = 0;
    unsigned short warningBleepsLeft = 0;

    void animateStatusLED();
    void decreaseWarningBleeps();

public:
    explicit IV18Animator(IV18Display &display);
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    void doWarning(short bleepsCount = 10);
    void doKillLED();
    void doFrame();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
