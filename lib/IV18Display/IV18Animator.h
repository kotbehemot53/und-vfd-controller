#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18Display.h"

class IV18Animator
{
private:
    static const short PREPARE_MIN_DUTY_US = 150;
    static const short GRID_MAX_DUTY_US = 961;

    static const short LED_HEARTBEAT = 0;
    static const short LED_WARNING = 1;
    static const short LED_SINUS_MODES_COUNT = 2;

    static const short LED_KILL = 100;

    static const unsigned long FRAME_LENGTH_US = 10000; // us -> just below 1/100 s

    static constexpr unsigned short FRAMES_PER_CYCLE[LED_SINUS_MODES_COUNT] = {150, 30};
    static constexpr unsigned long MIN_DUTY_US[LED_SINUS_MODES_COUNT] = {1000, 0};
    static constexpr unsigned long MAX_DUTY_US[LED_SINUS_MODES_COUNT] = {7000, 9000};

    static const unsigned short LAMP_GRID_FRAMES_PER_CYCLE = 75;

    DeviceAnimator animator;
    unsigned short currentFrame = 0;

    // TODO: how to make it static and initialize it? preprocessor?
    unsigned short framesPerLongestCycle;

    DeviceAnimatorThread* threads;
    DeviceAnimatorThread* statusLedThread;
    DeviceAnimatorThread* lampGridThread;

    unsigned short ledAction = 0;
    unsigned short warningBleepsLeft = 0;

    unsigned short randomLampGridOrder[IV18Display::GRID_STEPS_COUNT] = {1, 3, 5, 7, 9, 11, 13, 15, 17};
//    unsigned short lampCycleNumber = 0;

    void animateStatusLED();
    void animateGridsBrightness();
    void decreaseWarningBleeps();

//    static int randomCompare(const void *cmp1, const void *cmp2);
    static void shuffleArray(unsigned short * array, int size);

public:
    explicit IV18Animator(IV18Display &display);
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    void doWarning(short bleepsCount = 10);
    void doKillLED();
    void doFrame();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
