#include "IV18Animator.h"

IV18Animator::IV18Animator(IV18Display &display)
{
    heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateHeartbeat
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 4500),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 4500)
    };

    blinkingSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateHeartbeat
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 4500),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 4500)
    };

    lampGridSteps = new DeviceAnimatorStep[IV18Display::GRID_STEPS_COUNT*2]{
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 0),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 1),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 2),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 3),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 4),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 5),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 6),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 7),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 8)
    };

    threads = new DeviceAnimatorThread[3]{
        DeviceAnimatorThread(heartbeatSteps, 2, false),
        DeviceAnimatorThread(blinkingSteps, 2, false),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };

    heartbeatThread = &(threads[0]);
    blinkingThread = &(threads[1]);
    lampGridThread = &(threads[2]);

    animator.setThreads(threads, 3);
}

void IV18Animator::animateHeartbeat()
{
    // don't do anything if heartbeat disabled
    if (!heartbeatThread->isEnabled) {
        return;
    }

    // TODO: think about adding a variable for lowering the top brightness
    // formula for a nice sinusoidal bajinga
    //((FRAME_LENGTH_US - DIMMEST_HEARTBEAT_ON_TIME_US)/2)* (1 - cos(x/(0.0506599 * FRAMES_PER_HEARTBEAT_CYCLE * π))) + DIMMEST_HEARTBEAT_ON_TIME_US

    // TODO: think about calculating intermediate value of dutyCycle, stored as animator's property
    //       and modifiable from outside? or is it overkill?

    short onTime;
    short offTime;
    double multiplier = 1 - cos(currentFrame/(0.0506599 * FRAMES_PER_HEARTBEAT_CYCLE * PI));
    onTime = multiplier <= 0 ? DIMMEST_HEARTBEAT_ON_TIME_US : ((FRAME_LENGTH_US - DIMMEST_HEARTBEAT_ON_TIME_US) / 2) * multiplier + DIMMEST_HEARTBEAT_ON_TIME_US;
    offTime = (long) FRAME_LENGTH_US < onTime ? 0 : FRAME_LENGTH_US - onTime;

    // TODO: maybe make a waitUs setter, only getters for the rest and make them private?
    heartbeatSteps[0].waitUs = onTime;
    heartbeatSteps[1].waitUs = offTime;
}

void IV18Animator::doFrame()
{
    // multiframe animation of the heartbeat
    animateHeartbeat();

    animator.doFrame();

    ++currentFrame;

    // for now, we're doing uniform cycles per all threads (applies only to heartbeat atm)
    if (currentFrame >= FRAMES_PER_HEARTBEAT_CYCLE) {
        currentFrame = 0;
    }
}
