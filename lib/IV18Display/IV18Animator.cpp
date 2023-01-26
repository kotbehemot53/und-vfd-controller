#include "IV18Animator.h"

IV18Animator::IV18Animator(IV18Display &display)
{
    heartbeatSteps = new DeviceAnimatorStep[2]{
        // waitUs values will be overriden on every frame by animateHeartbeat
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOn), 5000),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::statusOff), 5000)
    };

    // TODO: do we actually need the empty frame?
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
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 8),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::prepareNextGridStep), 100, 9),
        DeviceAnimatorStep(&display, reinterpret_cast<void (*)(void*,int)>(&IV18AnimationSteps::doGridStep), 900, 9),
    };

    threads = new DeviceAnimatorThread[2]{
        DeviceAnimatorThread(heartbeatSteps, 2),
        DeviceAnimatorThread(lampGridSteps, IV18Display::GRID_STEPS_COUNT*2)
    };

    animator.setThreads(threads, 2);
}

void IV18Animator::animateHeartbeat()
{
    // formula for a nice sinusoidal bajinga
    //((FRAME_LENGTH_US - DIMMEST_LENGTH_US)/2)* (1 - cos(x/(0.0506599 * FRAMES_PER_CYCLE * π))) + DIMMEST_LENGTH_US

    short onTime;
    short offTime;
    double multiplier = 1 - cos(currentFrame/(0.0506599 * FRAMES_PER_CYCLE * PI));
    onTime = multiplier <= 0 ? DIMMEST_LENGTH_US : ((FRAME_LENGTH_US - DIMMEST_LENGTH_US) / 2) * multiplier + DIMMEST_LENGTH_US;
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
    if (currentFrame >= FRAMES_PER_CYCLE) {
        currentFrame = 0;
    }
}
