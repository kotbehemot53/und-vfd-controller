#ifndef PIUCNTVFD1_IV18ANIMATOR_H
#define PIUCNTVFD1_IV18ANIMATOR_H

#include "../DutyCycleGenerator/SinusoidalDutyCycleGenerator.h"
#include "../DeviceAnimator/DeviceAnimator.h"
#include "IV18AnimationSteps.h"
#include "IV18I2CCommandExecutor.h"
#include "IV18Display.h"

/**
 * Main animator class for the IV18Display. Animates the digits and the status LED. Can run intros, fade digits in and out.
 * Governs each frame of the main loop.
 * It uses the DeviceAnimator internally to perform the animation on the concrete display.
 */
class IV18Animator
{
public:
    // available status led animation modes
    static const short LED_MODE_HEARTBEAT = 0; // smooth sinusoidal heartbeat (may cause undertimes and clash with dimmed digits!)
    static const short LED_MODE_WARNING = 1; // quick sinusoidal bleeps (lasts only for the bleep count set in warningBleepsLeft)
    static const short LED_MODE_SQUARE_HEARTBEAT = 3; // square heartbeat (less undertimes and no visible clashing with dimmed digits)
    static const short LED_MODE_DIM = 100; // led is dim

    static const short LED_MODE_DEFAULT = LED_MODE_SQUARE_HEARTBEAT;

    // available lamp grid animation modes
    static const short LAMP_MODE_DIGIT_STATIC = 0;    // constant state
    static const short LAMP_MODE_DIGIT_IN = 1;        // fade-in
    static const short LAMP_MODE_DIGIT_OUT = 2;       // fade-out

    // lamp grid duty cycle bounds
    static const short LAMP_DIGIT_PREPARE_MIN_DUTY_US = 150;
    static const short LAMP_DIGIT_MAX_DUTY_US = 961;
    static const short LAMP_DIGIT_CUTOUT_DUTY_US = 100;

    static const unsigned short DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE = 100;
private:
    static const unsigned long LED_FRAME_LENGTH_US = 10000; // 1/100 s

    static const short LED_SINUS_MODES_COUNT = 2;

    static constexpr unsigned short LED_FRAMES_PER_CYCLE[LED_SINUS_MODES_COUNT] = {150, 30};
    static const unsigned short LED_FRAMES_PER_CYCLE_MAX = 300; // LED_FRAMES_PER_CYCLE values should be divisors of this one!

    static constexpr unsigned long LED_MIN_DUTY_US[LED_SINUS_MODES_COUNT] = {500, 0};
    static constexpr unsigned long LED_MAX_DUTY_US[LED_SINUS_MODES_COUNT] = {7000, 9000};
    static const unsigned long LED_MAX_DUTY_SQUARE_US = 1000;
    static const unsigned long LED_MIN_DUTY_SQUARE_US = 500;
    static const unsigned long LED_DUTY_DIM_US = 400;

    IV18Display* display;
    DeviceAnimator* animator;
    DeviceAnimatorThread* statusLedThread;
    DeviceAnimatorThread* lampDigitThread;

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

    unsigned short lampDigitActions[IV18Display::DIGIT_STEPS_COUNT] = {
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
        LAMP_MODE_DIGIT_STATIC,
    };

    unsigned short ledCurrentFrameInCycle = 0;

    DeviceAnimatorThread* threads;

    unsigned short ledAction = LED_MODE_DEFAULT;
    unsigned short warningBleepsLeft = 0;

    void (*sequencingCallback)(IV18Animator* animator);
    bool sequencingEnabled = false;

    void animateStatusLED();
    void decreaseWarningBleeps();

    void animateLampDigitBrightnesses();

public:

    explicit IV18Animator(IV18Display* display);

    /**
     * Sets a listener to notify when there's an undertime in the animation. Attaches it to the nested DeviceAnimator.
     *
     * @param failureListener
     */
    void setFailureListener(AnimatorFailureListenerInterface* failureListener);

    /**
     * Do a frame of the current animation.
     */
    void doFrame();

    /**
     * Do current sequencing (based on the current sequencingCallback set). Prepares a sequence for a following set of frames.
     * It may be used for intros and other complex animations.
     */
    void doCurrentSequencing();

    // TODO: maybe use this INSIDE the functions that require it?
    /**
     * Converts byte duty cycle (value from 0 to 255) to actual time in US required by
     * setLampDigitAction() & setCurrentLampDigitValue()
     *
     * @param byte dutyCycle A value between 0 and 255
     * @return short On-duty time in microseconds.
     */
    inline static short convertDutyCycle(byte dutyCycle)
    {
        return LAMP_DIGIT_CUTOUT_DUTY_US + (dutyCycle/255.0 * (LAMP_DIGIT_MAX_DUTY_US - LAMP_DIGIT_CUTOUT_DUTY_US));
    }

    /**
     * Get the on-duty time for the previous frame, for a given digit.
     *
     * @param lampDigitNumber
     *
     * @return Number of microseconds
     */
    unsigned short getLampDigitPreviousOnDutyUs(byte lampDigitNumber);

    /**
     * Set the on-duty time for a given digit.
     *
     * @param lampDigitNumber
     *
     * @param dutyValue Time in microseconds
     */
    void setCurrentLampDigitDutyValue(short lampDigitNumber, unsigned short dutyValue);

    /**
     * Set the action for a lamp digit.
     *
     * @param lampDigitNumber
     * @param action the action; set to one of: LAMP_MODE_DIGIT_STATIC (static shine), LAMP_MODE_DIGIT_IN (fade-in), LAMP_MODE_DIGIT_OUT (fade-out).
     * @param maxDutyValue Max on-duty time for the given action (in microseconds).
     * @param minDutyValue Min on-duty time for the given action (in microseconds).
     */
    void setLampDigitAction(short lampDigitNumber, short action, unsigned short maxDutyValue = LAMP_DIGIT_MAX_DUTY_US, unsigned short minDutyValue = LAMP_DIGIT_CUTOUT_DUTY_US);

    /**
     * Set the number of frames for the current action on a given digit.
     *
     * @param lampDigitNumber
     * @param lampDigitFramesPerAction How many frames should the current action take (works for fade-in and fade-out).
     */
    void setLampDigitFramesPerAction(short lampDigitNumber, unsigned short lampDigitFramesPerAction = DEFAULT_LAMP_DIGIT_FRAMES_PER_CYCLE);

    /**
     * Sets the current sequencing callback (see doCurrentSequencing). Can be used for intros and other complex animations.
     *
     * @param sequencingCallbackToSet
     */
    void setSequencingCallback(void (*sequencingCallbackToSet)(IV18Animator*));

    /**
     * Disables sequencing (see doCurrentSequencing).
     */
    void disableSequencing();

    /**
     * Runs a warning animation on the status LED.
     *
     * @param bleepsCount How many bleeps?
     */
    void setLEDModeWarning(short bleepsCount = 10);

    /**
     * Sets status LED to a smooth sinusoidal heartbeat (may cause undertimes and clash with dimmed digits!)
     */
    void setLEDMode(short ledMode);

    /**
     * Returns the display object pointer.
     *
     * @return IV18Display*
     */
    IV18Display* getDisplay();
};

#endif //PIUCNTVFD1_IV18ANIMATOR_H
