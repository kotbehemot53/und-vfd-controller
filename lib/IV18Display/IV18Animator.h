#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DutyCycleGenerator/SinusoidalDutyCycleGenerator.h"
#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18I2CCommandExecutor.h"
#include "IV18Display.h"

class IV18Animator
{
public:
    // available status led animation modes
    static const short LED_HEARTBEAT = 0;
    static const short LED_WARNING = 1;

    // available lamp grid animation modes
    static const short LAMP_DIGIT_STATIC = 0;    // constant state
    static const short LAMP_DIGIT_IN = 1;        // fade-in
    static const short LAMP_DIGIT_OUT = 2;       // fade-out

    // lamp grid duty cycle bounds
    static const short LAMP_DIGIT_PREPARE_MIN_DUTY_US = 150;
    static const short LAMP_DIGIT_MAX_DUTY_US = 961;
    static const short LAMP_DIGIT_CUTOUT_DUTY_US = 100;

    static const unsigned short DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE = 100;
private:
    static const unsigned long LED_FRAME_LENGTH_US = 10000; // 1/100 s

    static const short LED_SINUS_MODES_COUNT = 2;

    static const short LED_KILL = 100;

    static constexpr unsigned short LED_FRAMES_PER_CYCLE[LED_SINUS_MODES_COUNT] = {150, 30};
    static constexpr unsigned long LED_MIN_DUTY_US[LED_SINUS_MODES_COUNT] = {500, 0};
    static constexpr unsigned long LED_MAX_DUTY_US[LED_SINUS_MODES_COUNT] = {7000, 9000};

    IV18Display* display;

//    static const unsigned short LAMP_GRID_FRAMES_PER_CYCLE = 75;
    // TODO: add setter for this to use via command
    //       recalculate lampGridFramesPerLongestCycle on set
    unsigned short lampDigitFramesPerCycle[IV18Display::DIGIT_STEPS_COUNT] = {
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE,
        DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE
    };

    unsigned short lampDigitCurrentFrameInCycle[IV18Display::DIGIT_STEPS_COUNT] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0};

//    // holds previous frame duty cycle values for each digit (for reading)
//    unsigned short lampDigitPreviousOnDutyUs[IV18Display::DIGIT_STEPS_COUNT] = {
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US,
//        LAMP_DIGIT_MAX_DUTY_US
//    };

    // sets max duty cycle per lamp grid (permanent dimming) for fade-in/fade-out
    unsigned short lampDigitMaxOnDutyUs[IV18Display::DIGIT_STEPS_COUNT] = {
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US,
        LAMP_DIGIT_MAX_DUTY_US
    };

    // sets min duty cycle per lamp grid (permanent dimming) for fade-in/fade-out
    unsigned short lampDigitMinOffDutyUs[IV18Display::DIGIT_STEPS_COUNT] = {
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US,
        LAMP_DIGIT_CUTOUT_DUTY_US
    };

    // TODO: add setter for this to use via command
    unsigned short lampDigitActions[IV18Display::DIGIT_STEPS_COUNT] = {
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
        LAMP_DIGIT_STATIC,
    };

    // TODO: methods to init lamp grid modes/animations, animate them properly, set on duty values (regular/permanent dimming)

    DeviceAnimator animator;
    unsigned short ledCurrentFrame = 0;

    // TODO: how to make it static and initialize it? preprocessor?
    unsigned short ledFramesPerLongestCycle;
//    unsigned short lampGridFramesPerLongestCycle;

    DeviceAnimatorThread* threads;

    unsigned short ledAction = 0;
    unsigned short warningBleepsLeft = 0;

    void (*sequencingCallback)(IV18Animator* animator);
    bool sequencingEnabled = false;

    void animateStatusLED();
    void decreaseWarningBleeps();

    // TODO: do we need this silliness?
//    unsigned short randomLampGridOrder[IV18Display::DIGIT_STEPS_COUNT] = {1, 3, 5, 7, 9, 11, 13, 15, 17};
//    unsigned short lampCycleNumber = 0;
//    static int randomCompare(const void *cmp1, const void *cmp2);
//    static void shuffleArray(unsigned short * array, int size);

    void animateLampDigitBrightnesses();

public:
    // TODO: make appropriate getters/setters instead?
    DeviceAnimatorThread* statusLedThread;
    DeviceAnimatorThread* lampDigitThread;

    explicit IV18Animator(IV18Display* display);
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    void doWarning(short bleepsCount = 10);
    void doKillLED();
    void doFrame();
    void doCurrentSequencing();

//    void setLampDigitOnDutyValues(const unsigned short * values);

    // TODO: maybe use this INSIDE the functions that require it?
    /**
     * Converts byte duty cycle (value from 0 to 255) to actual time in US required by
     * setLampDigitAction() & setCurrentLampDigitValue()
     *
     * @param byte dutyCycle
     * @return
     */
    inline static short convertDutyCycle(byte dutyCycle)
    {
        return LAMP_DIGIT_CUTOUT_DUTY_US + (dutyCycle/255.0 * (LAMP_DIGIT_MAX_DUTY_US - LAMP_DIGIT_CUTOUT_DUTY_US));
    }

    unsigned short getLampDigitPreviousOnDutyUs(byte lampGridNumber);

    void setCurrentLampDigitDutyValue(short lampGridNumber, unsigned short dutyValue);
    void setLampDigitAction(short lampDigitNumber, short action, unsigned short maxDutyValue = LAMP_DIGIT_MAX_DUTY_US, unsigned short minDutyValue = LAMP_DIGIT_CUTOUT_DUTY_US);
    void setLampDigitFramesPerAction(short lampDigitNumber, unsigned short lampDigitFramesPerAction = DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE);
    void setSequencingCallback(void (*sequencingCallbackToSet)(IV18Animator*));
    void disableSequencing();

    IV18Display* getDisplay();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
