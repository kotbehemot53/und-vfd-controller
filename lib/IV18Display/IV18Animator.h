#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18Display.h"

class IV18Animator
{
private:
    static const unsigned long FRAME_LENGTH_US = 10000; // us -> just below 1/100 s

    static const short LED_HEARTBEAT = 0;
    static const short LED_WARNING = 1;
    static const short LED_SINUS_MODES_COUNT = 2;

    static const short LED_KILL = 100;

    static constexpr unsigned short LED_FRAMES_PER_CYCLE[LED_SINUS_MODES_COUNT] = {150, 30};
    static constexpr unsigned long LED_MIN_DUTY_US[LED_SINUS_MODES_COUNT] = {500, 0};
    static constexpr unsigned long LED_MAX_DUTY_US[LED_SINUS_MODES_COUNT] = {7000, 9000};

    // lamp grid duty cycle bounds
    static const short LAMP_GRID_PREPARE_MIN_DUTY_US = 150;
    static const short LAMP_GRID_MAX_DUTY_US = 961;

    // available lamp grid animation modes
    static const short LAMP_GRID_OFF = 0;   // constant off
    static const short LAMP_GRID_ON = 1;    // constant on
    static const short LAMP_GRID_IN = 2;    // phase in
    static const short LAMP_GRID_OUT = 3;   // phase out

//    static const unsigned short LAMP_GRID_FRAMES_PER_CYCLE = 75;
    unsigned short lampGridFramesPerCycle[IV18Display::GRID_STEPS_COUNT] =
        {150, 150, 150, 150, 150, 150, 150, 150, 150};
    unsigned short lampGridCurrentFrameInCycle[IV18Display::GRID_STEPS_COUNT] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned short lampGridOnDutyUs[IV18Display::GRID_STEPS_COUNT] = {
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US,
        LAMP_GRID_MAX_DUTY_US
    };

    // TODO: methods to init lamp grid modes/animations, animate them properly, set on duty values (regular/permanent dimming)

    DeviceAnimator animator;
    unsigned short currentFrame = 0; // TODO: rename to ledCurrentFrame?

    // TODO: how to make it static and initialize it? preprocessor?
    unsigned short framesPerLongestCycle;

    DeviceAnimatorThread* threads;
    DeviceAnimatorThread* statusLedThread;
    DeviceAnimatorThread* lampGridThread;

    unsigned short ledAction = 0;
    unsigned short warningBleepsLeft = 0;

    void animateStatusLED();
    void decreaseWarningBleeps();

    // TODO: do we need this silliness?
//    unsigned short randomLampGridOrder[IV18Display::GRID_STEPS_COUNT] = {1, 3, 5, 7, 9, 11, 13, 15, 17};
//    unsigned short lampCycleNumber = 0;
//    static int randomCompare(const void *cmp1, const void *cmp2);
//    static void shuffleArray(unsigned short * array, int size);
//    void animateGridsBrightness();

public:
    explicit IV18Animator(IV18Display &display);
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    void doWarning(short bleepsCount = 10);
    void doKillLED();
    void doFrame();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
